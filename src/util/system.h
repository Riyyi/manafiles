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
	System();
	virtual ~System() {}

	enum FileDescriptor {
		ReadFileDescriptor,
		WriteFileDescriptor,
	};

	System operator()();
	System operator()(const char* command);
	System operator()(std::string command);
	System operator()(std::string_view command);
	System operator()(const std::vector<const char*>& arguments);
	System operator()(const std::vector<std::string>& arguments);
	System operator()(const std::vector<std::string_view>& arguments);

	System operator+(System rhs);
	System operator|(System rhs);
	System operator||(System rhs);
	System operator&&(System rhs);

	void print(const std::vector<std::string>& arguments);

	const std::vector<std::string>& arguments() const { return m_arguments; }
	std::string output() const { return m_output; }
	std::string error() const { return m_error; }
	int status() const { return m_status; }

private:
	System(const std::vector<std::string>& arguments);

	System exec(std::string input = "");
	void readFromFileDescriptor(int fileDescriptor[2], std::string& output);

	std::vector<std::string> m_arguments;
	std::string m_output;
	std::string m_error;
	int m_status { 0 };
};

} // namespace Util

#endif // SYSTEM_H
