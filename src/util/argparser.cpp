/*
 * Copyright (C) 2021-2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#include <algorithm> // find_if
#include <cstddef>   // size_t
#include <cstdint>   // uint8_t
#include <cstdio>    // printf
#include <cstring>   // strcmp
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

void ArgParser::printError(char parameter, Error error)
{
	char tmp[] { parameter, '\0' };
	printError(tmp, error, false);
}

void ArgParser::printError(const char* parameter, Error error, bool longName)
{
	if (!m_errorReporting) {
		return;
	}

	if (error == Error::OptionInvalid) {
		printf("%s: invalid option -- '%s'\n", m_name, parameter);
	}
	else if (error == Error::OptionUnrecognized) {
		printf("%s: unrecognized option -- '%s'\n", m_name, parameter);
	}
	else if (error == Error::OptionDoesntAllowArgument) {
		printf("%s: option '--%s' doesn't allow an argument\n", m_name, parameter);
	}
	else if (error == Error::OptionRequiresArgument) {
		if (longName) {
			printf("%s: option '--%s' requires an argument\n", m_name, parameter);
		}
		else {
			printf("%s: option requires an argument -- '%s'\n", m_name, parameter);
		}
	}
	else if (error == Error::OptionInvalidArgumentType) {
		if (longName) {
			printf("%s: option '--%s' invalid type\n", m_name, parameter);
		}
		else {
			printf("%s: option invalid type -- '%s'\n", m_name, parameter);
		}
	}
	else if (error == Error::ArgumentExtraOperand) {
		printf("%s: extra operand '%s'\n", m_name, parameter);
	}
	else if (error == Error::ArgumentRequired) {
		printf("%s: missing required argument '%s'\n", m_name, parameter);
	}
	else if (error == Error::ArgumentInvalidType) {
		printf("%s: invalid argument type '%s'\n", m_name, parameter);
	}

	// TODO: Print command usage, if it's enabled.
}

// Required: directly after || separated by space
// Optional: directly after
bool ArgParser::parseShortOption(std::string_view option, std::string_view next)
{
	bool result = true;

#ifndef NDEBUG
	printf("Parsing short option: '%s'\n", option.data());
#endif

	char c;
	std::string_view value;
	for (size_t i = 0; i < option.size(); ++i) {
		c = option.at(i);

#ifndef NDEBUG
		printf("short '%c'\n", c);
#endif

		auto foundOption = std::find_if(m_options.begin(), m_options.end(), [&c](Option& it) -> bool {
			return it.shortName == c;
		});

		// Option does not exist
		if (foundOption == m_options.cend()) {
			printError(c, Error::OptionInvalid);

			if (m_exitOnFirstError) {
				return false;
			}
		}
		else if (foundOption->requiresArgument == Required::No) {
			// FIXME: Figure out why providing a nullptr breaks the lambda here.
			result = foundOption->acceptValue("");
		}
		else if (foundOption->requiresArgument == Required::Yes) {
			value = option.substr(i + 1);
			if (value.empty() && next.empty()) {
				foundOption->error = Error::OptionRequiresArgument;
				printError(c, Error::OptionRequiresArgument);
				result = false;
			}
			else if (!value.empty()) {
				result = foundOption->acceptValue(value.data());
				if (!result) {
					printError(c, Error::OptionInvalidArgumentType);
				}
			}
			else if (next[0] == '-') {
				foundOption->error = Error::OptionRequiresArgument;
				printError(c, Error::OptionRequiresArgument);
				result = false;
			}
			else {
				result = foundOption->acceptValue(next.data());
				m_optionIndex++;
				if (!result) {
					printError(c, Error::OptionInvalidArgumentType);
				}
			}

			break;
		}
		else if (foundOption->requiresArgument == Required::Optional) {
			value = option.substr(i + 1);
			if (!value.empty()) {
				result = foundOption->acceptValue(value.data());
				if (!result) {
					printError(c, Error::OptionInvalidArgumentType);
				}
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
	std::string name = std::string(option.substr(0, option.find_first_of('=')));
	std::string_view value = option.substr(option.find_first_of('=') + 1);

	auto foundOption = std::find_if(m_options.begin(), m_options.end(), [&name](Option& it) -> bool {
		return it.longName && it.longName == name;
	});

	if (foundOption == m_options.cend()) {
		printError(name.data(), Error::OptionUnrecognized);
		return false;
	}

	enum class ArgumentProvided : uint8_t {
		No,
		DirectlyAfter,
		Seperated,
	};

	auto argument = ArgumentProvided::No;
	if (name != value || option.find('=') != std::string_view::npos) {
		argument = ArgumentProvided::DirectlyAfter;
	}
	else if (!next.empty() && next[0] != '-') {
		argument = ArgumentProvided::Seperated;
		value = next;
	}

	bool result = true;

	if (foundOption->requiresArgument == Required::No) {
		if (argument == ArgumentProvided::DirectlyAfter) {
			foundOption->error = Error::OptionDoesntAllowArgument;
			printError(name.data(), Error::OptionDoesntAllowArgument);
			return false;
		}

		result = foundOption->acceptValue("");
	}
	else if (foundOption->requiresArgument == Required::Yes) {
		if (argument == ArgumentProvided::No) {
			foundOption->error = Error::OptionRequiresArgument;
			printError(name.data(), Error::OptionRequiresArgument);
			return false;
		}

		result = foundOption->acceptValue(value.data());
		if (!result) {
			printError(name.data(), Error::OptionInvalidArgumentType);
		}

		if (argument == ArgumentProvided::Seperated) {
			m_optionIndex++;
		}
	}
	else if (foundOption->requiresArgument == Required::Optional) {
		if (argument == ArgumentProvided::DirectlyAfter) {
			result = foundOption->acceptValue(value.data());
			if (!result) {
				printError(name.data(), Error::OptionInvalidArgumentType);
			}
		}
	}

	return result;
}

bool ArgParser::parseArgument(std::string_view argument)
{
	bool result = true;

	for (;;) {
		// Run out of argument handlers
		if (m_argumentIndex >= m_arguments.size()) {
			printError(argument.data(), Error::ArgumentExtraOperand);
			return false;
		}

		Argument& currentArgument = m_arguments.at(m_argumentIndex);
		result = currentArgument.acceptValue(argument.data());

		if (result) {
			currentArgument.addedValues++;
			if (currentArgument.addedValues >= currentArgument.maxValues) {
				m_argumentIndex++;
			}
		}
		else if (currentArgument.minValues == 0) {
			m_argumentIndex++;
			continue;
		}
		else {
			printError(argument.data(), Error::ArgumentInvalidType);
		}

		break;
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

#ifndef NDEBUG
		printf("argv[%zu]: %s\n", m_optionIndex, argv[m_optionIndex]);
#endif

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
			printError(currentArgument.name ? currentArgument.name : "", Error::ArgumentRequired);
		}
	}

	if (result) {
		return true;
	}

	for (auto& option : m_options) {
		if (option.longName && strcmp(option.longName, "help") == 0) {
			printf("Try '%s --help' for more information.\n", m_name);
			break;
		}
		if (option.shortName == 'h') {
			printf("Try '%s -h' for more information.\n", m_name);
			break;
		}
	}

	return false;
}

// -----------------------------------------

void ArgParser::addOption(Option&& option)
{
	m_options.push_back(option);
}

void ArgParser::addOption(bool& value, char shortName, const char* longName, const char* usageString, const char* manString)
{
	addOption({ shortName, longName, nullptr, usageString, manString, Required::No, getAcceptFunction(value) });
}

void ArgParser::addOption(const char*& value, char shortName, const char* longName, const char* usageString, const char* manString, const char* argumentName, Required requiresArgument)
{
	addOption({ shortName, longName, argumentName, usageString, manString, requiresArgument, getAcceptFunction(value) });
}

void ArgParser::addOption(std::string& value, char shortName, const char* longName, const char* usageString, const char* manString, const char* argumentName, Required requiresArgument)
{
	addOption({ shortName, longName, argumentName, usageString, manString, requiresArgument, getAcceptFunction(value) });
}

void ArgParser::addOption(std::string_view& value, char shortName, const char* longName, const char* usageString, const char* manString, const char* argumentName, Required requiresArgument)
{
	addOption({ shortName, longName, argumentName, usageString, manString, requiresArgument, getAcceptFunction(value) });
}

void ArgParser::addOption(int& value, char shortName, const char* longName, const char* usageString, const char* manString, const char* argumentName, Required requiresArgument)
{
	addOption({ shortName, longName, argumentName, usageString, manString, requiresArgument, getAcceptFunction(value) });
}

void ArgParser::addOption(unsigned int& value, char shortName, const char* longName, const char* usageString, const char* manString, const char* argumentName, Required requiresArgument)
{
	addOption({ shortName, longName, argumentName, usageString, manString, requiresArgument, getAcceptFunction(value) });
}

void ArgParser::addOption(double& value, char shortName, const char* longName, const char* usageString, const char* manString, const char* argumentName, Required requiresArgument)
{
	addOption({ shortName, longName, argumentName, usageString, manString, requiresArgument, getAcceptFunction(value) });
}

void ArgParser::addOption(std::vector<std::string>& values, char shortName, const char* longName, const char* usageString, const char* manString, const char* argumentName, Required requiresArgument)
{
	addOption({ shortName, longName, argumentName, usageString, manString, requiresArgument, getAcceptFunction(values) });
}

// -----------------------------------------

void ArgParser::addArgument(Argument&& argument)
{
	m_arguments.push_back(argument);
}

void ArgParser::addArgument(bool& value, const char* name, const char* usageString, const char* manString, Required required)
{
	size_t minValues = required == Required::Yes ? 1 : 0;
	addArgument({ name, usageString, manString, minValues, 1, 0, getAcceptFunction(value) });
}

void ArgParser::addArgument(const char*& value, const char* name, const char* usageString, const char* manString, Required required)
{
	size_t minValues = required == Required::Yes ? 1 : 0;
	addArgument({ name, usageString, manString, minValues, 1, 0, getAcceptFunction(value) });
}

void ArgParser::addArgument(std::string& value, const char* name, const char* usageString, const char* manString, Required required)
{
	size_t minValues = required == Required::Yes ? 1 : 0;
	addArgument({ name, usageString, manString, minValues, 1, 0, getAcceptFunction(value) });
}

void ArgParser::addArgument(std::string_view& value, const char* name, const char* usageString, const char* manString, Required required)
{
	size_t minValues = required == Required::Yes ? 1 : 0;
	addArgument({ name, usageString, manString, minValues, 1, 0, getAcceptFunction(value) });
}

void ArgParser::addArgument(int& value, const char* name, const char* usageString, const char* manString, Required required)
{
	size_t minValues = required == Required::Yes ? 1 : 0;
	addArgument({ name, usageString, manString, minValues, 1, 0, getAcceptFunction(value) });
}

void ArgParser::addArgument(unsigned int& value, const char* name, const char* usageString, const char* manString, Required required)
{
	size_t minValues = required == Required::Yes ? 1 : 0;
	addArgument({ name, usageString, manString, minValues, 1, 0, getAcceptFunction(value) });
}

void ArgParser::addArgument(double& value, const char* name, const char* usageString, const char* manString, Required required)
{
	size_t minValues = required == Required::Yes ? 1 : 0;
	addArgument({ name, usageString, manString, minValues, 1, 0, getAcceptFunction(value) });
}

void ArgParser::addArgument(std::vector<std::string>& values, const char* name, const char* usageString, const char* manString, Required required)
{
	size_t minValues = required == Required::Yes ? 1 : 0;
	addArgument({ name, usageString, manString, minValues, values.max_size(), 0, getAcceptFunction(values) });
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
	return [&value](const char* input) -> bool {
		value = input;
		return true;
	};
}

AcceptFunction ArgParser::getAcceptFunction(std::string& value)
{
	return [&value](const char* input) -> bool {
		value = input;
		return true;
	};
}

AcceptFunction ArgParser::getAcceptFunction(std::string_view& value)
{
	return [&value](const char* input) -> bool {
		value = input;
		return true;
	};
}

AcceptFunction ArgParser::getAcceptFunction(int& value)
{
	return [&value](const char* input) -> bool {
		const char* validate = input;
		for (; *validate != '\0'; ++validate) {
			// - [0-9]
			if (*validate != 45 && (*validate < 48 || *validate > 57)) {
				return false;
			}
		}

		try {
			value = std::stoi(input);
			return true;
		}
		catch (...) {
			return false;
		}
	};
}

AcceptFunction ArgParser::getAcceptFunction(unsigned int& value)
{
	return [&value](const char* input) -> bool {
		const char* validate = input;
		for (; *validate != '\0'; ++validate) {
			// [0-9]
			if (*validate < 48 || *validate > 57) {
				return false;
			}
		}

		unsigned long convert = 0;
		try {
			convert = std::stoul(input);
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
	return [&value](const char* input) -> bool {
		const char* validate = input;
		for (; *validate != '\0'; ++validate) {
			// . [0-9]
			if (*validate != 46 && (*validate < 48 || *validate > 57)) {
				return false;
			}
		}

		try {
			value = std::stod(input);
			return true;
		}
		catch (...) {
			return false;
		}
	};
}

AcceptFunction ArgParser::getAcceptFunction(std::vector<std::string>& value)
{
	return [&value](const char* input) -> bool {
		value.push_back(input);
		return true;
	};
}

} // namespace Util
