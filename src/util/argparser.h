#ifndef ARG_PARSER_H
#define ARG_PARSER_H

#include <functional> // function
#include <string>
#include <string_view>
#include <vector>

namespace Util {

class ArgParser final {
public:
	ArgParser();
	virtual ~ArgParser();

	enum class Required {
		No,
		Yes,
		Optional,
	};

	enum class Error {
		None,
		InvalidOption,      // For short options
		UnrecognizedOption, // For long options
		DoesntAllowArgument,
		RequiresArgument,
	};

	struct Option {
		char shortName { 0 };
		const char* longName { nullptr };
		const char* argumentName { nullptr };
		const char* usageString { nullptr };
		const char* manString { nullptr };
		Required requiresArgument;
		std::function<bool(const char*)> acceptValue;

		Error error = Error::None;
	};

	struct Argument {
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
	void addOption(std::vector<std::string>& value, char shortName, const char* longName, const char* usageString, const char* manString, const char* argumentName = "", Required requiresArgument = Required::No);

	void setErrorReporting(bool state) { m_errorReporting = state; }
	void setExitOnFirstError(bool state) { m_exitOnFirstError = state; }
	void setStopParsingOnFirstNonOption(bool state) { m_stopParsingOnFirstNonOption = state; }

private:
	void printOptionError(char name, Error error);
	void printOptionError(const char* name, Error error, bool longName = true);
	bool parseShortOption(std::string_view option, std::string_view next);
	bool parseLongOption(std::string_view option, std::string_view next);

	bool m_errorReporting { true };
	bool m_exitOnFirstError { true };
	bool m_stopParsingOnFirstNonOption { false };

	int m_optionIndex { 1 };
	bool m_nonOptionMode { false };

	const char* m_name;
	std::vector<Option> m_options;
	std::vector<Argument> m_arguments;
};

} // namespace Util

#endif // ARG_PARSER_H
