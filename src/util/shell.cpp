#include <cstdio> // pclose, perror, popen
#include <string>
#include <string_view>

#include "util/shell.h"

namespace Util {

Shell::Shell()
{
}

Shell::Shell(const std::string& output, int status)
	: m_output(output)
	, m_status(status)
{
}

Shell Shell::operator()(const char* command)
{
	FILE* shell = popen(command, "r");
	if (!shell) {
		perror("\033[31;1mError:\033[0m popen");
		return { "", -1 };
	}

	std::string output;

	constexpr int bufferSize = 4096;
	char buffer[bufferSize];
	while (fgets(buffer, sizeof(buffer), shell)) {
		output.append(buffer);
	}

	int status = pclose(shell);
	if (status < 0) {
		perror("\033[31;1mError:\033[0m pclose");
	}

	return { output, status };
}

Shell Shell::operator()(std::string command)
{
	return operator()(command.c_str());
}

Shell Shell::operator()(std::string_view command)
{
	return operator()(command.data());
}

} // namespace Util
