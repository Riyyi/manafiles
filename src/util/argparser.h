/*
 * Copyright (C) 2022 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstddef>    // size_t
#include <cstdint>    // uint8_t
#include <functional> // function
#include <string>
#include <string_view>
#include <vector>

namespace Util {

using AcceptFunction = std::function<bool(const char*)>;

class ArgParser final {
public:
	ArgParser();
	virtual ~ArgParser();

	enum class Required : uint8_t {
		No,
		Yes,
		Optional,
	};

	enum class Error : uint8_t {
		None,
		OptionInvalid,      // For short options
		OptionUnrecognized, // For long options
		OptionDoesntAllowArgument,
		OptionRequiresArgument,
		OptionInvalidArgumentType,
		ArgumentExtraOperand,
		ArgumentRequired,
		ArgumentInvalidType,
	};

	struct Option {
		char shortName { 0 };
		const char* longName { nullptr };
		const char* argumentName { nullptr };
		const char* usageString { nullptr };
		const char* manString { nullptr };
		Required requiresArgument;
		AcceptFunction acceptValue;

		Error error = Error::None;
	};

	struct Argument {
		const char* name { nullptr };
		const char* usageString { nullptr };
		const char* manString { nullptr };
		size_t minValues { 0 };
		size_t maxValues { 1 };
		size_t addedValues { 0 };
		AcceptFunction acceptValue;
	};

	bool parse(int argc, const char* argv[]);

	void addOption(Option&& option);
	void addOption(bool& value, char shortName, const char* longName, const char* usageString, const char* manString);
	void addOption(const char*& value, char shortName, const char* longName, const char* usageString, const char* manString, const char* argumentName = "", Required requiresArgument = Required::No);
	void addOption(std::string& value, char shortName, const char* longName, const char* usageString, const char* manString, const char* argumentName = "", Required requiresArgument = Required::No);
	void addOption(std::string_view& value, char shortName, const char* longName, const char* usageString, const char* manString, const char* argumentName = "", Required requiresArgument = Required::No);
	void addOption(int& value, char shortName, const char* longName, const char* usageString, const char* manString, const char* argumentName = "", Required requiresArgument = Required::No);
	void addOption(unsigned int& value, char shortName, const char* longName, const char* usageString, const char* manString, const char* argumentName = "", Required requiresArgument = Required::No);
	void addOption(double& value, char shortName, const char* longName, const char* usageString, const char* manString, const char* argumentName = "", Required requiresArgument = Required::No);
	void addOption(std::vector<std::string>& values, char shortName, const char* longName, const char* usageString, const char* manString, const char* argumentName = "", Required requiresArgument = Required::No);

	void addArgument(Argument&& argument);
	void addArgument(bool& value, const char* name, const char* usageString, const char* manString, Required required = Required::No);
	void addArgument(const char*& value, const char* name, const char* usageString, const char* manString, Required required = Required::No);
	void addArgument(std::string& value, const char* name, const char* usageString, const char* manString, Required required = Required::No);
	void addArgument(std::string_view& value, const char* name, const char* usageString, const char* manString, Required required = Required::No);
	void addArgument(int& value, const char* name, const char* usageString, const char* manString, Required required = Required::No);
	void addArgument(unsigned int& value, const char* name, const char* usageString, const char* manString, Required required = Required::No);
	void addArgument(double& value, const char* name, const char* usageString, const char* manString, Required required = Required::No);
	void addArgument(std::vector<std::string>& values, const char* name, const char* usageString, const char* manString, Required required = Required::No);

	void setErrorReporting(bool state) { m_errorReporting = state; }
	void setExitOnFirstError(bool state) { m_exitOnFirstError = state; }
	void setStopParsingOnFirstNonOption(bool state) { m_stopParsingOnFirstNonOption = state; }

private:
	void printError(char parameter, Error error);
	void printError(const char* parameter, Error error, bool longName = true);
	bool parseShortOption(std::string_view option, std::string_view next);
	bool parseLongOption(std::string_view option, std::string_view next);
	bool parseArgument(std::string_view argument);

	AcceptFunction getAcceptFunction(bool& value);
	AcceptFunction getAcceptFunction(const char*& value);
	AcceptFunction getAcceptFunction(std::string& value);
	AcceptFunction getAcceptFunction(std::string_view& value);
	AcceptFunction getAcceptFunction(int& value);
	AcceptFunction getAcceptFunction(unsigned int& value);
	AcceptFunction getAcceptFunction(double& value);
	AcceptFunction getAcceptFunction(std::vector<std::string>& value);

	bool m_errorReporting { true };
	bool m_exitOnFirstError { true };
	bool m_stopParsingOnFirstNonOption { false };

	size_t m_optionIndex { 1 };
	size_t m_argumentIndex { 0 };
	bool m_nonOptionMode { false };

	const char* m_name { nullptr };
	std::vector<Option> m_options;
	std::vector<Argument> m_arguments;
};

} // namespace Util
