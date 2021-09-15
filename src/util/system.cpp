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

void System::operator()(const char* command)
{
	// Split modifies the string, so two copies are needed
	char charCommand[strlen(command)];
	strcpy(charCommand, command);
	char charCommand2[strlen(command)];
	strcpy(charCommand2, command);

	size_t index = split(charCommand, " ");

	// Preallocation is roughly ~10% faster even with just a single space.
	std::vector<char*> arguments(index + 1, 0);

	split(charCommand2, " ", [&arguments](int index, char* pointer) {
		arguments[index] = pointer;
	});

	operator()(arguments);
}

void System::operator()(std::string command)
{
	operator()(command.c_str());
}

void System::operator()(std::string_view command)
{
	operator()(command.data());
}

void System::operator()(const std::vector<const char*>& arguments)
{
	std::vector<char*> nonConstArguments(arguments.size() + 1, 0);
	for (size_t i = 0; i < arguments.size(); ++i) {
		nonConstArguments[i] = const_cast<char*>(arguments[i]);
	}
	operator()(nonConstArguments);
}

void System::operator()(const std::vector<std::string>& arguments)
{
	std::vector<char*> nonConstArguments(arguments.size() + 1, 0);
	for (size_t i = 0; i < arguments.size(); ++i) {
		nonConstArguments[i] = const_cast<char*>(arguments[i].c_str());
	}

	operator()(nonConstArguments);
}

void System::operator()(const std::vector<std::string_view>& arguments)
{
	std::vector<char*> nonConstArguments(arguments.size() + 1, 0);
	for (size_t i = 0; i < arguments.size(); ++i) {
		nonConstArguments[i] = const_cast<char*>(arguments[i].data());
	}
	operator()(nonConstArguments);
}

// -----------------------------------------

void System::operator()(const std::vector<char*>& arguments)
{
	int stdoutFd[2];
	int stderrFd[2];
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
		close(stdoutFd[ReadFileDescriptor]);
		dup2(stdoutFd[WriteFileDescriptor], fileno(stdout));
		close(stdoutFd[WriteFileDescriptor]);

		close(stderrFd[ReadFileDescriptor]);
		dup2(stderrFd[WriteFileDescriptor], fileno(stderr));
		close(stderrFd[WriteFileDescriptor]);

		execvp(arguments[0], &arguments[0]);
		exit(0);
	}
	// Parent
	default:
		break;
	}

	readFromFileDescriptor(stdoutFd, m_output);
	readFromFileDescriptor(stderrFd, m_error);

	int result;
	do {
		result = waitpid(pid, &m_status, 0);
	} while (result == -1 && errno == EINTR);
	m_status = WEXITSTATUS(m_status);
}

void System::readFromFileDescriptor(int fileDescriptor[2], std::string& output)
{
	close(fileDescriptor[WriteFileDescriptor]);

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

size_t System::split(char* split, const char* delimiters, SplitCallback callback)
{
	size_t index = 0;
	char* pointer = strtok(split, delimiters);
	while (pointer != nullptr) {
		if (callback) {
			callback(index, pointer);
		}
		index++;
		pointer = strtok(nullptr, delimiters);
	}

	return index;
}

} // namespace Util
