#ifndef ARG_PARSER_H
#define ARG_PARSER_H

#include <functional>
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
	void addOption(std::string& value, char shortName, const char* longName, const char* usageString, const char* manString, const char* argumentName = "", Required requiresArgument = Required::No);
	void addOption(std::vector<std::string>& value, char shortName, const char* longName, const char* usageString, const char* manString, const char* argumentName = "", Required requiresArgument = Required::No);

	void setOptionIndex(int index) { m_optionIndex = index; }
	void setExitOnFirstError(bool state) { m_exitOnFirstError = state; }
	void setErrorMessages(bool state) { m_errorMessages = state; }
	void setNonOptionAfterFirst(bool state) { m_nonOptionAfterFirst = state; }

private:
	void printOptionError(char name, Error error);
	void printOptionError(const char* name, Error error, bool longName = true);
	bool parseShortOption(std::string_view option, std::string_view next);
	bool parseLongOption(std::string_view option, std::string_view next);

	int m_optionIndex { 1 };
	bool m_nonOptionMode { false };
	bool m_exitOnFirstError { true };
	bool m_errorMessages { true };
	// TODO: Implement this, maybe combine with error messages flag, enum class? or bitfield
	bool m_nonOptionAfterFirst { false };

	const char* m_name;
	std::vector<Option> m_options;
	std::vector<Argument> m_arguments;
};

} // namespace Util

#endif // ARG_PARSER_H
