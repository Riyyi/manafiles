#ifndef SYSTEM_H
#define SYSTEM_H

#include <cstddef>    // size_t
#include <functional> // function
#include <string>
#include <string_view>
#include <vector>

namespace Util {

using SplitCallback = std::function<void(size_t, char*)>;

class System {
public:
	System() {}
	virtual ~System() {}

	enum FileDescriptor {
		ReadFileDescriptor,
		WriteFileDescriptor,
	};

	void operator()(const char* command);
	void operator()(std::string command);
	void operator()(std::string_view command);
	void operator()(const std::vector<const char*>& arguments);
	void operator()(const std::vector<std::string>& arguments);
	void operator()(const std::vector<std::string_view>& arguments);

	std::string output() const { return m_output; }
	std::string error() const { return m_error; }
	int status() const { return m_status; }

private:
	void operator()(const std::vector<char*>& arguments);
	void readFromFileDescriptor(int fileDescriptor[2], std::string& output);
	size_t split(char* split, const char* delimiters, SplitCallback callback = {});

	std::string m_output;
	std::string m_error;
	int m_status { 0 };
};

} // namespace Util

#endif // SYSTEM_H
