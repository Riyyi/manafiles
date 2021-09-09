#include <algorithm> // find_if
#include <cstddef>   // size_t
#include <cstdio>    // printf
#include <limits>    // numeric_limits
#include <string>    // stod, stoi, stoul
#include <string_view>
#include <vector>

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
	if (!m_errorReporting) {
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
	for (size_t i = 0; i < option.size(); ++i) {
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
		else if (foundOption->requiresArgument == Required::No) {
			// FIXME: Figure out why providing a nullptr breaks the lambda here.
			result = foundOption->acceptValue("");
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
				result = foundOption->acceptValue(value.data());
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
				result = foundOption->acceptValue(next.data());
				m_optionIndex++;
			}

			break;
		}
		else if (foundOption->requiresArgument == Required::Optional) {
			value = option.substr(i + 1);
			if (!value.empty()) {
				result = foundOption->acceptValue(value.data());
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
		printOptionError(name.data(), Error::UnrecognizedOption);

		result = false;
	}
	else if (argumentProvided) {
		if (foundOption->requiresArgument == Required::No) {
			foundOption->error = Error::DoesntAllowArgument;
			printOptionError(name.data(), Error::DoesntAllowArgument);

			result = false;
		}
		else if (foundOption->requiresArgument == Required::Yes) {
			result = foundOption->acceptValue(value.data());
		}
		else if (foundOption->requiresArgument == Required::Optional) {
			result = foundOption->acceptValue(value.data());
		}
	}
	else if (!next.empty() && foundOption->requiresArgument == Required::Yes) {
		if (next[0] == '-') {
			foundOption->error = Error::RequiresArgument;
			printOptionError(name.data(), Error::RequiresArgument);

			result = false;
		}
		else {
			result = foundOption->acceptValue(next.data());
			m_optionIndex++;
		}
	}
	else if (foundOption->requiresArgument == Required::Yes) {
		foundOption->error = Error::RequiresArgument;
		printOptionError(name.data(), Error::RequiresArgument);

		result = false;
	}
	else {
		// FIXME: Figure out why providing a nullptr breaks the lambda here.
		result = foundOption->acceptValue("");
	}

	return result;
}

bool ArgParser::parseArgument(std::string_view argument)
{
	// No handler for argument
	if (m_argumentIndex >= m_arguments.size()) {
		return false;
	}

	Argument& currentArgument = m_arguments.at(m_argumentIndex);
	bool result = currentArgument.acceptValue(argument.data());

	if (result) {
		currentArgument.addedValues++;
	}

	if (currentArgument.addedValues >= currentArgument.maxValues) {
		m_argumentIndex++;
	}

	return result;
}

bool ArgParser::parse(int argc, const char* argv[])
{
	bool result = true;

	// Set looping indices
	m_optionIndex = 1;
	m_argumentIndex = 0;

	// By default parse all '-' prefixed parameters as options
	m_nonOptionMode = false;

	// Get program name
	m_name = argv[0] + std::string_view(argv[0]).find_last_of('/') + 1;

	std::string_view argument;
	std::string_view next;
	for (; m_optionIndex < (size_t)argc; ++m_optionIndex) {
		printf("argv[%zu]: %s\n", m_optionIndex, argv[m_optionIndex]);

		// Get the current and next parameter
		argument = argv[m_optionIndex];
		if (m_optionIndex + 1 < (size_t)argc && argv[m_optionIndex + 1][0] != '-') {
			next = argv[m_optionIndex + 1];
		}
		else {
			next = {};
		}

		// Stop parsing '-' prefixed parameters as options afer '--'
		if (argument.compare("--") == 0) {
			m_nonOptionMode = true;
			continue;
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
			if (m_stopParsingOnFirstNonOption) {
				m_nonOptionMode = true;
			}
			if (!parseArgument(argument)) {
				result = false;
			}
		}

		if (m_exitOnFirstError && !result) {
			break;
		}
	}

	// Check any leftover arguments for required
	for (; m_argumentIndex < m_arguments.size(); ++m_argumentIndex) {
		Argument& currentArgument = m_arguments.at(m_argumentIndex);
		if (currentArgument.minValues > currentArgument.addedValues) {
			result = false;
			// TODO: decide if and what to print here
		}
	}

	return result;
}

// -----------------------------------------

AcceptFunction ArgParser::getAcceptFunction(bool& value)
{
	return [&value](const char*) -> bool {
		value = true;
		return true;
	};
}

AcceptFunction ArgParser::getAcceptFunction(const char*& value)
{
	return [&value](const char* v) -> bool {
		value = v;
		return true;
	};
}

AcceptFunction ArgParser::getAcceptFunction(std::string& value)
{
	return [&value](const char* v) -> bool {
		value = v;
		return true;
	};
}

AcceptFunction ArgParser::getAcceptFunction(std::string_view& value)
{
	return [&value](const char* v) -> bool {
		value = v;
		return true;
	};
}

AcceptFunction ArgParser::getAcceptFunction(int& value)
{
	return [&value](const char* v) -> bool {
		try {
			value = std::stoi(v);
			return true;
		}
		catch (...) {
			return false;
		}
	};
}

AcceptFunction ArgParser::getAcceptFunction(unsigned int& value)
{
	return [&value](const char* v) -> bool {
		unsigned long convert = 0;
		try {
			convert = std::stoul(v);
		}
		catch (...) {
			return false;
		}

		if (convert <= std::numeric_limits<unsigned int>::max()) {
			value = static_cast<unsigned int>(convert);
			return true;
		}

		return false;
	};
}

AcceptFunction ArgParser::getAcceptFunction(double& value)
{
	return [&value](const char* v) -> bool {
		try {
			value = std::stod(v);
			return true;
		}
		catch (...) {
			return false;
		}
	};
}

AcceptFunction ArgParser::getAcceptFunction(std::vector<std::string>& value)
{
	return [&value](const char* v) -> bool {
		value.push_back(v);
		return true;
	};
}

// -----------------------------------------

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
		getAcceptFunction(value),
	};
	addOption(std::move(option));
}

void ArgParser::addOption(const char*& value, char shortName, const char* longName, const char* usageString, const char* manString, const char* argumentName, Required requiresArgument)
{
	Option option {
		shortName,
		longName,
		argumentName,
		usageString,
		manString,
		requiresArgument,
		getAcceptFunction(value),
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
		getAcceptFunction(value),
	};
	addOption(std::move(option));
}

void ArgParser::addOption(std::string_view& value, char shortName, const char* longName, const char* usageString, const char* manString, const char* argumentName, Required requiresArgument)
{
	Option option {
		shortName,
		longName,
		argumentName,
		usageString,
		manString,
		requiresArgument,
		getAcceptFunction(value),
	};
	addOption(std::move(option));
}

void ArgParser::addOption(int& value, char shortName, const char* longName, const char* usageString, const char* manString, const char* argumentName, Required requiresArgument)
{
	Option option {
		shortName,
		longName,
		argumentName,
		usageString,
		manString,
		requiresArgument,
		getAcceptFunction(value),
	};
	addOption(std::move(option));
}

void ArgParser::addOption(unsigned int& value, char shortName, const char* longName, const char* usageString, const char* manString, const char* argumentName, Required requiresArgument)
{
	Option option {
		shortName,
		longName,
		argumentName,
		usageString,
		manString,
		requiresArgument,
		getAcceptFunction(value),
	};
	addOption(std::move(option));
}

void ArgParser::addOption(double& value, char shortName, const char* longName, const char* usageString, const char* manString, const char* argumentName, Required requiresArgument)
{
	Option option {
		shortName,
		longName,
		argumentName,
		usageString,
		manString,
		requiresArgument,
		getAcceptFunction(value),
	};
	addOption(std::move(option));
}

void ArgParser::addOption(std::vector<std::string>& values, char shortName, const char* longName, const char* usageString, const char* manString, const char* argumentName, Required requiresArgument)
{
	Option option {
		shortName,
		longName,
		argumentName,
		usageString,
		manString,
		requiresArgument,
		getAcceptFunction(values),
	};
	addOption(std::move(option));
}

// -----------------------------------------

void ArgParser::addArgument(Argument&& argument)
{
	m_arguments.push_back(argument);
}

void ArgParser::addArgument(bool& value, const char* name, const char* usageString, const char* manString, Required required)
{
	size_t minValues = required == Required::Yes ? 1 : 0;
	Argument argument {
		name,
		usageString,
		manString,
		minValues,
		1,
		0,
		getAcceptFunction(value),
	};
	addArgument(std::move(argument));
}

void ArgParser::addArgument(const char*& value, const char* name, const char* usageString, const char* manString, Required required)
{
	size_t minValues = required == Required::Yes ? 1 : 0;
	Argument argument {
		name,
		usageString,
		manString,
		minValues,
		1,
		0,
		getAcceptFunction(value),
	};
	addArgument(std::move(argument));
}

void ArgParser::addArgument(std::string& value, const char* name, const char* usageString, const char* manString, Required required)
{
	size_t minValues = required == Required::Yes ? 1 : 0;
	Argument argument {
		name,
		usageString,
		manString,
		minValues,
		1,
		0,
		getAcceptFunction(value),
	};
	addArgument(std::move(argument));
}

void ArgParser::addArgument(std::string_view& value, const char* name, const char* usageString, const char* manString, Required required)
{
	size_t minValues = required == Required::Yes ? 1 : 0;
	Argument argument {
		name,
		usageString,
		manString,
		minValues,
		1,
		0,
		getAcceptFunction(value),
	};
	addArgument(std::move(argument));
}

void ArgParser::addArgument(int& value, const char* name, const char* usageString, const char* manString, Required required)
{
	size_t minValues = required == Required::Yes ? 1 : 0;
	Argument argument {
		name,
		usageString,
		manString,
		minValues,
		1,
		0,
		getAcceptFunction(value),
	};
	addArgument(std::move(argument));
}

void ArgParser::addArgument(unsigned int& value, const char* name, const char* usageString, const char* manString, Required required)
{
	size_t minValues = required == Required::Yes ? 1 : 0;
	Argument argument {
		name,
		usageString,
		manString,
		minValues,
		1,
		0,
		getAcceptFunction(value),
	};
	addArgument(std::move(argument));
}

void ArgParser::addArgument(double& value, const char* name, const char* usageString, const char* manString, Required required)
{
	size_t minValues = required == Required::Yes ? 1 : 0;
	Argument argument {
		name,
		usageString,
		manString,
		minValues,
		1,
		0,
		getAcceptFunction(value),
	};
	addArgument(std::move(argument));
}

void ArgParser::addArgument(std::vector<std::string>& values, const char* name, const char* usageString, const char* manString, Required required)
{
	size_t minValues = required == Required::Yes ? 1 : 0;
	Argument argument {
		name,
		usageString,
		manString,
		minValues,
		values.max_size(),
		0,
		getAcceptFunction(values),
	};
	addArgument(std::move(argument));
}

} // namespace Util
