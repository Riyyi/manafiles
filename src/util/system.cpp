#include <cerrno>     // errno, EAGAIN, EINTR
#include <cstddef>    // size_t
#include <cstdio>     // perror
#include <cstdlib>    // exit, WEXITSTATUS
#include <cstring>    // strcpy, strtok
#include <functional> // function
#include <string>
#include <string_view>
#include <sys/wait.h> // waitpid
#include <unistd.h>   // close, dup2, execvp, fork, pipe, read
#include <vector>

#include "util/system.h"

namespace Util {

System::System()
{
}

System::System(std::vector<std::string> arguments)
	: m_arguments(arguments)
{
}

System System::operator()()
{
	return exec();
}

System System::operator()(const char* command)
{
	return operator()(std::string { command });
}

System System::operator()(std::string command)
{
	std::vector<std::string> arguments;

	size_t index = 0;
	while (index != std::string::npos) {
		index = command.find_first_of(" ");
		arguments.push_back(command.substr(0, index));
		command = command.substr(index + 1);
	}

	return System(arguments);
}

System System::operator()(std::string_view command)
{
	return operator()(std::string { command });
}

System System::operator()(const std::vector<const char*>& arguments)
{
	std::vector<std::string> stringArguments(arguments.size(), "");
	for (size_t i = 0; i < arguments.size(); ++i) {
		stringArguments[i] = arguments[i];
	}

	return System(stringArguments);
}

System System::operator()(const std::vector<std::string>& arguments)
{
	return System(arguments);
}

System System::operator()(const std::vector<std::string_view>& arguments)
{
	std::vector<std::string> stringArguments(arguments.size(), "");
	for (size_t i = 0; i < arguments.size(); ++i) {
		stringArguments[i] = arguments[i];
	}

	return System(stringArguments);
}

System System::operator|(System rhs)
{
	rhs.exec(exec().output());
	return rhs;
}

void System::print(const std::vector<std::string>& arguments)
{
	if (!arguments.size()) {
		return;
	}

	printf("----------\n");
	printf("size:    %zu\n", arguments.size());
	printf("command: ");
	for (size_t i = 0; i < arguments.size(); ++i) {
		printf("%s ", arguments.at(i).c_str());
	}
	printf("\n");
	printf("----------\n");
}

// -----------------------------------------

System System::exec(std::string input)
{
	int stdinFd[2];
	int stdoutFd[2];
	int stderrFd[2];
	if (pipe(stdinFd) < 0) {
		perror("\033[31;1mError\033[0m");
	}
	if (pipe(stdoutFd) < 0) {
		perror("\033[31;1mError\033[0m");
	}
	if (pipe(stderrFd) < 0) {
		perror("\033[31;1mError\033[0m");
	}

	pid_t pid = fork();
	switch (pid) {
	// Failed
	case -1:
		perror("\033[31;1mError\033[0m");
		break;
	// Child
	case 0: {
		close(stdinFd[WriteFileDescriptor]);
		dup2(stdinFd[ReadFileDescriptor], fileno(stdin));
		close(stdinFd[ReadFileDescriptor]);

		close(stdoutFd[ReadFileDescriptor]);
		dup2(stdoutFd[WriteFileDescriptor], fileno(stdout));
		close(stdoutFd[WriteFileDescriptor]);

		close(stderrFd[ReadFileDescriptor]);
		dup2(stderrFd[WriteFileDescriptor], fileno(stderr));
		close(stderrFd[WriteFileDescriptor]);

		std::vector<char*> charArguments(m_arguments.size() + 1, 0);
		for (size_t i = 0; i < m_arguments.size(); ++i) {
			charArguments[i] = const_cast<char*>(m_arguments[i].c_str());
		}

		execvp(charArguments[0], &charArguments[0]);
		exit(0);
	}
	// Parent
	default:
		break;
	}

	close(stdinFd[ReadFileDescriptor]);
	if (!input.empty()) {
		write(stdinFd[WriteFileDescriptor], input.c_str(), input.size());
	}
	close(stdinFd[WriteFileDescriptor]);

	readFromFileDescriptor(stdoutFd, m_output);
	readFromFileDescriptor(stderrFd, m_error);

	int result;
	do {
		result = waitpid(pid, &m_status, 0);
	} while (result == -1 && errno == EINTR);
	m_status = WEXITSTATUS(m_status);

	return *this;
}

void System::readFromFileDescriptor(int fileDescriptor[2], std::string& output)
{
	close(fileDescriptor[WriteFileDescriptor]);
	output.clear();

	constexpr int bufferSize = 4096;
	char buffer[bufferSize];

	do {
		const ssize_t result = read(fileDescriptor[ReadFileDescriptor], buffer, bufferSize);
		// FIXME: also handle failure cases
		if (result > 0) {
			output.append(buffer, result);
		}
	} while (errno == EAGAIN || errno == EINTR);

	if (!output.empty() && output.find_last_of('\n') == output.size() - 1) {
		output.pop_back();
	}

	close(fileDescriptor[ReadFileDescriptor]);
}

} // namespace Util
