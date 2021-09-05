#include <algorithm> // std::find_if
#include <cstdio> // printf
#include <string_view>

#include "util/argparser.h"

namespace Util {

ArgParser::ArgParser()
{
}

ArgParser::~ArgParser()
{
}

void ArgParser::printOptionError(char name, Error error)
{
	char tmp[] { name, '\0' };
	printOptionError(tmp, error, false);
}

void ArgParser::printOptionError(const char* name, Error error, bool longName)
{
	if (!m_errorMessages) {
		return;
	}

	if (error == Error::InvalidOption) {
		printf("%s: invalid option -- '%s'\n", m_name, name);
	}
	else if (error == Error::UnrecognizedOption) {
		printf("%s: unrecognized option -- '%s'\n", m_name, name);
	}
	else if (error == Error::DoesntAllowArgument) {
		printf("%s: option '--%s' doesn't allow an argument\n", m_name, name);
	}
	else if (error == Error::RequiresArgument) {
		if (longName) {
			printf("%s: option '--%s' requires an argument", m_name, name);
		}
		else {
			printf("%s: option requires an argument -- '%s'\n", m_name, name);
		}
	}

	// TODO: Print command usage, if it's enabled.
}

// Required: directly after || separated by space
// Optional: directly after
bool ArgParser::parseShortOption(std::string_view option, std::string_view next)
{
	bool result = true;

	printf("Parsing short option: '%s'\n", option.data());

	char c;
	std::string_view value;
	for (std::string_view::size_type i = 0; i < option.size(); ++i) {
		c = option.at(i);
		printf("short '%c'\n", c);

		auto foundOption = std::find_if(m_options.begin(), m_options.end(), [&c](Option& it) -> bool {
			return it.shortName == c;
		});

		// Option does not exist
		if (foundOption == m_options.cend()) {
			printOptionError(c, Error::InvalidOption);

			result = false;
			if (m_exitOnFirstError) {
				return result;
			}
		}

		if (foundOption->requiresArgument == Required::No) {
			// FIXME: Figure out why providing a nullptr breaks the lambda here.
			foundOption->acceptValue("");
		}
		else if (foundOption->requiresArgument == Required::Yes) {
			value = option.substr(i + 1);
			if (value.empty() && next.empty()) {
				foundOption->error = Error::RequiresArgument;
				printOptionError(c, Error::RequiresArgument);

				result = false;
				if (m_exitOnFirstError) {
					return result;
				}
			}
			else if (!value.empty()) {
				foundOption->acceptValue(value.data());
			}
			else if (next[0] == '-') {
				foundOption->error = Error::RequiresArgument;
				printOptionError(c, Error::RequiresArgument);

				result = false;
				if (m_exitOnFirstError) {
					return result;
				}
			}
			else {
				foundOption->acceptValue(next.data());
				m_optionIndex++;
			}

			break;
		}
		else if (foundOption->requiresArgument == Required::Optional) {
			value = option.substr(i + 1);
			if (!value.empty()) {
				foundOption->acceptValue(value.data());
				break;
			}
		}
	}

	return result;
}

// Required: directly after, separated by '=' || separated by space
// Optional: directly after, separated by '='
bool ArgParser::parseLongOption(std::string_view option, std::string_view next)
{
	bool result = true;

	std::string_view name = option.substr(0, option.find_first_of('='));
	std::string_view value = option.substr(option.find_first_of('=') + 1);

	bool argumentProvided = true;
	if (name.compare(value) == 0 && option.find('=') == std::string_view::npos) {
		argumentProvided = false;
	}

	printf("Parsing long option: '%s' with value '%s'\n", name.data(), argumentProvided ? value.data() : "");

	auto foundOption = std::find_if(m_options.begin(), m_options.end(), [&name](Option& it) -> bool {
		return it.longName == name;
	});

	if (foundOption == m_options.cend()) {
		foundOption->error = Error::UnrecognizedOption;
		printOptionError(name.data(), Error::UnrecognizedOption);

		result = false;
		if (m_exitOnFirstError) {
			return result;
		}
	}

	if (argumentProvided) {
		if (foundOption->requiresArgument == Required::No) {
			foundOption->error = Error::DoesntAllowArgument;
			printOptionError(name.data(), Error::DoesntAllowArgument);

			result = false;
			if (m_exitOnFirstError) {
				return result;
			}
		}
		else if (foundOption->requiresArgument == Required::Yes) {
			foundOption->acceptValue(value.data());
		}
		else if (foundOption->requiresArgument == Required::Optional) {
			foundOption->acceptValue(value.data());
		}
	}
	else if (!next.empty() && foundOption->requiresArgument == Required::Yes) {
		if (next[0] == '-') {
			foundOption->error = Error::RequiresArgument;
			printOptionError(name.data(), Error::RequiresArgument);

			result = false;
			if (m_exitOnFirstError) {
				return result;
			}
		}
		else {
			foundOption->acceptValue(next.data());
			m_optionIndex++;
		}
	}
	else if (foundOption->requiresArgument == Required::Yes) {
		foundOption->error = Error::RequiresArgument;
		printOptionError(name.data(), Error::RequiresArgument);

		result = false;
		if (m_exitOnFirstError) {
			return result;
		}
	}
	else {
		// FIXME: Figure out why providing a nullptr breaks the lambda here.
		foundOption->acceptValue("");
	}

	return result;
}

bool ArgParser::parse(int argc, const char* argv[])
{
	bool result = true;

	// By default parse all '-' prefixed parameters as options
	m_nonOptionMode = false;

	// Get program name
	m_name = argv[0] + std::string_view(argv[0]).find_last_of('/') + 1;

	printf("name: %s\n", m_name);

	std::string_view argument;
	std::string_view next;
	for (; m_optionIndex < argc; ++m_optionIndex) {
		printf("argv[%d]: %s\n", m_optionIndex, argv[m_optionIndex]);

		// Get the current and next parameter
		argument = argv[m_optionIndex];
		if (m_optionIndex + 1 < argc && argv[m_optionIndex + 1][0] != '-') {
			next = argv[m_optionIndex + 1];
		}
		else {
			next = {};
		}

		// Stop parsing '-' prefixed parameters as options afer '--'
		if (argument.compare("--") == 0) {
			m_nonOptionMode = true;
		}

		// Long Option
		if (!m_nonOptionMode && argument[0] == '-' && argument[1] == '-') {
			argument = argument.substr(argument.find_first_not_of('-'));
			if (!parseLongOption(argument, next)) {
				result = false;
			}
		}
		// Short Option
		else if (!m_nonOptionMode && argument[0] == '-') {
			argument = argument.substr(argument.find_first_not_of('-'));
			if (!parseShortOption(argument, next)) {
				result = false;
			}
		}
		// Argument
		else {
			printf("-> argu: '%s'", argument.data());
		}
	}

	return result;
}

void ArgParser::addOption(Option&& option)
{
	m_options.push_back(option);
}

void ArgParser::addOption(bool& value, char shortName, const char* longName, const char* usageString, const char* manString)
{
	Option option {
		shortName,
		longName,
		nullptr,
		usageString,
		manString,
		Required::No,
		[&value](const char*) -> bool {
			value = true;
			return true;
		}
	};
	addOption(std::move(option));
}

void ArgParser::addOption(std::string& value, char shortName, const char* longName, const char* usageString, const char* manString, const char* argumentName, Required requiresArgument)
{
	Option option {
		shortName,
		longName,
		argumentName,
		usageString,
		manString,
		requiresArgument,
		[&value](const char* a) -> bool {
			value = a;
			return true;
		}
	};
	addOption(std::move(option));
}

} // namespace Util
