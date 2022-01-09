#include <cstdio> // fprintf, perror, stderr
#include <filesystem>
#include <string>
#include <unistd.h> // gethostname
#include <vector>

#include "dotfile.h"
#include "package.h"
#include "util/argparser.h"
#include "util/file.h"
#include "util/system.h"
#include "util/timer.h"

// void* operator new(size_t size)
// {
// 	std::cout << "@Allocating '" << size << "' bytes" << std::endl;
// 	return std::malloc(size);
// }

// void operator delete(void* pointer, size_t size)
// {
// 	std::cout << "@Freeing '" << size << "' bytes" << std::endl;
// 	free(pointer);
// }

int main(int argc, const char* argv[])
{
	bool fileOperation = false;
	bool packageOperation = false;
	bool helpOperation = false;

	bool addOrAur = false;
	bool install = false;
	bool pull = false;
	bool pushOrStore = false;

	std::vector<std::string> targets {};

	Util::ArgParser argParser;
	argParser.addOption(fileOperation, 'F', "file", nullptr, nullptr);
	argParser.addOption(packageOperation, 'P', "package", nullptr, nullptr);
	argParser.addOption(helpOperation, 'h', "help", nullptr, nullptr);

	argParser.addOption(addOrAur, 'a', "add", nullptr, nullptr);
	argParser.addOption(install, 'i', "install", nullptr, nullptr);
	argParser.addOption(pull, 'l', "pull", nullptr, nullptr);
	argParser.addOption(pushOrStore, 's', "push", nullptr, nullptr);

	argParser.addArgument(targets, "targets", nullptr, nullptr, Util::ArgParser::Required::No);
	argParser.parse(argc, argv);

	if (fileOperation + packageOperation + helpOperation > 1) {
		fprintf(stderr, "\033[31;1mError:\033[0m only one operation may be used at a time\n");
		return 1;
	}

	char hostname[64] { 0 };
	if (gethostname(hostname, 64) < 0) {
		perror("\033[31;1mError:\033[0m gethostname");
	}

	Util::Timer t;

	if (fileOperation) {
		Dotfile dotfile;

		Dotfile::setExcludePaths({
			{ Dotfile::ExcludeType::File, "dotfiles.sh" },
			{ Dotfile::ExcludeType::File, "packages" },
			{ Dotfile::ExcludeType::EndsWith, ".md" },
			{ Dotfile::ExcludeType::EndsWith, "README.org" },
			{ Dotfile::ExcludeType::Directory, ".git" },
			{ Dotfile::ExcludeType::File, "screenshot.png" },

			{ Dotfile::ExcludeType::Directory, ".cache" },
			{ Dotfile::ExcludeType::Directory, "CMakeFiles" },
			{ Dotfile::ExcludeType::Directory, "cppcheck-cppcheck-build-dir" },
		});

		Dotfile::setSystemDirectories({ "/boot", "/etc", "/usr/share" });
		Dotfile::setWorkingDirectory(std::filesystem::current_path());

		if (addOrAur) {
			dotfile.add(targets);
		}
		if (pull) {
			dotfile.pull(targets);
		}
		if (pushOrStore) {
			dotfile.push(targets);
		}
		if (!addOrAur && !pull && !pushOrStore) {
			dotfile.list(targets);
		}
	}
	else if (packageOperation) {
		Package package;

		Package::setHostname(hostname);

		if (addOrAur) {
			// TODO install tracked AUR packages
		}
		if (install) {
			// TODO install tracked repo packages
		}
		if (pushOrStore) {
			package.store();
		}
		if (!addOrAur && !install && !pushOrStore) {
			package.list(targets);
		}
	}
	else if (helpOperation) {
		// TODO open manpage
	}
	else {
		// TODO open manpage
	}

	printf("%fms\n", t.elapsedNanoseconds() / 1000000.0);

	return 0;
}

// cp -a <> <>
// -a = -dR --preserve=all
// -d = --no-dereference --preserve=links
// -P, --no-dereference = never follow symbolic links in SOURCE
// -R = recursive
// --preserve = preserve attributes (default: mode,ownership,timestamps), additional: context,links,xattr,all

// # Files that are stored in the repository but shouldn't get copied (regex)
// excludeFiles="${0#??}|$packageFile|.*.md$|.*README.org$|.git|screenshot.png"
// exclude: files, folders, ends-with
