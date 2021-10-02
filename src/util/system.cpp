#include <cerrno>     // errno, EAGAIN, EINTR
#include <cstddef>    // size_t
#include <cstdio>     // perror, ssize_t
#include <cstdlib>    // exit, WEXITSTATUS
#include <cstring>    // strcpy, strtok
#include <functional> // function
#include <sstream>    // istringstream
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

System::System(const std::vector<std::string>& arguments)
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

	return { arguments };
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

	return { stringArguments };
}

System System::operator()(const std::vector<std::string>& arguments)
{
	return { arguments };
}

System System::operator()(const std::vector<std::string_view>& arguments)
{
	std::vector<std::string> stringArguments(arguments.size(), "");
	for (size_t i = 0; i < arguments.size(); ++i) {
		stringArguments[i] = arguments[i];
	}

	return { stringArguments };
}

// Shell equivalent ;
System System::operator+(System rhs)
{
	auto lhs = *this;

	lhs.exec();
	rhs.m_output.append(lhs.m_output);
	rhs.m_error.append(lhs.m_error);
	rhs.exec();

	return rhs;
}

System System::operator|(System rhs)
{
	auto lhs = *this;

	lhs.exec();
	rhs.exec(lhs.m_output);

	return rhs;
}

System System::operator&&(System rhs)
{
	auto lhs = *this;

	lhs.exec();
	if (lhs.m_status > 0) {
		return lhs;
	}

	rhs.m_output.append(lhs.m_output);
	rhs.m_error.append(lhs.m_error);
	rhs.exec();

	return rhs;
}

System System::operator||(System rhs)
{
	auto lhs = *this;

	lhs.exec();
	if (lhs.m_status == 0) {
		return lhs;
	}

	rhs.m_output.append(lhs.m_output);
	rhs.m_error.append(lhs.m_error);
	rhs.exec();

	return rhs;
}

// cut -f -d
System& System::cut(uint32_t field, char delimiter)
{
	exec();

	return apply([&field, &delimiter](std::vector<std::string>& lines) {
		for (auto& line : lines) {
			size_t count = 1;
			size_t index = 0;
			while (index != std::string::npos) {
				if (count == field) {
					line = line.substr(0, line.find_first_of(delimiter));
					break;
				}

				index = line.find_first_of(delimiter);
				line = line.substr(index + 1);
				count++;
			}
		}
	});
}

System& System::sort(bool unique)
{
	exec();

	return apply([&unique](std::vector<std::string>& lines) {
		std::sort(lines.begin(), lines.end());

		if (unique) {
			auto last = std::unique(lines.begin(), lines.end());
			lines.erase(last, lines.end());
		}
	});
}

// tail -n
System& System::tail(int32_t number, bool starting)
{
	exec();

	return apply([&number, &starting](std::vector<std::string>& lines) {
		number = abs(number);
		if (!starting) {
			lines.erase(lines.begin(), lines.end() - number);
		}
		else {
			lines.erase(lines.begin(), lines.begin() + number - 1);
		}
	});
}

System& System::apply(LineCallback callback)
{
	exec();

	std::vector<std::string> lines;

	auto stream = std::istringstream(m_output);
	std::string line;
	while (std::getline(stream, line)) {
		lines.push_back(line);
	}

	callback(lines);

	m_output.clear();
	for (size_t i = 0; i < lines.size(); ++i) {
		m_output.append(lines.at(i) + '\n');
	}

	return *this;
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
	if (m_arguments.empty()) {
		return *this;
	}

	int stdinFd[2];
	int stdoutFd[2];
	int stderrFd[2];
	if (pipe(stdinFd) < 0) {
		perror("\033[31;1mError:\033[0m pipe");
	}
	if (pipe(stdoutFd) < 0) {
		perror("\033[31;1mError:\033[0m pipe");
	}
	if (pipe(stderrFd) < 0) {
		perror("\033[31;1mError:\033[0m pipe");
	}

	pid_t pid = fork();
	switch (pid) {
	// Failed
	case -1:
		perror("\033[31;1mError:\033[0m fork");
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
		m_arguments.clear();
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

	constexpr int bufferSize = 4096;
	char buffer[bufferSize];

	for (;;) {
		const ssize_t result = read(fileDescriptor[ReadFileDescriptor], buffer, bufferSize);
		if (result > 0) {
			output.append(buffer, result);
		}
		// EOF
		if (result == 0) {
			break;
		}
		// Error
		else if (result == -1) {
			if (errno != EAGAIN && errno != EINTR) {
				perror("\033[31;1mError:\033[0m read");
				break;
			}
		}
	}

	close(fileDescriptor[ReadFileDescriptor]);
}

} // namespace Util
