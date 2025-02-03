/*
 * Copyright (C) 2021-2022,2025 Riyyi
 *
 * SPDX-License-Identifier: MIT
 */

#include <cstdio> // fprintf, stderr
#include <string>
#include <vector>

#include "ruc/argparser.h"
#include "ruc/timer.h"

#include "config.h"
#include "dotfile.h"
#include "package.h"

int main(int argc, const char* argv[])
{
	bool fileOperation = false;
	bool packageOperation = false;
	bool helpOperation = false;

	bool addOrAur = false;
	bool install = false;
	bool pull = false;
	bool pushOrSearch = false;
	bool verbose = false;

	std::vector<std::string> targets {};

	ruc::ArgParser argParser;
	argParser.addOption(fileOperation, 'F', "file", nullptr, nullptr);
	argParser.addOption(packageOperation, 'P', "package", nullptr, nullptr);
	argParser.addOption(helpOperation, 'h', "help", nullptr, nullptr);

	argParser.addOption(addOrAur, 'a', "add", nullptr, nullptr);
	argParser.addOption(install, 'i', "install", nullptr, nullptr);
	argParser.addOption(pull, 'l', "pull", nullptr, nullptr);
	argParser.addOption(pushOrSearch, 's', "push", nullptr, nullptr);
	argParser.addOption(verbose, 'v', "verbose", nullptr, nullptr);

	argParser.addArgument(targets, "targets", nullptr, nullptr, ruc::ArgParser::Required::No);
	argParser.parse(argc, argv);

	if (fileOperation + packageOperation + helpOperation > 1) {
		fprintf(stderr, "\033[31;1mError:\033[0m only one operation may be used at a time\n");
		return 1;
	}

#ifndef NDEBUG
	ruc::Timer t;
#endif

	Config::the().setVerbose(verbose);

	if (fileOperation) {
		if (addOrAur) {
			Dotfile::the().add(targets);
		}
		if (pull) {
			Dotfile::the().pull(targets);
		}
		if (pushOrSearch) {
			Dotfile::the().push(targets);
		}
		if (!addOrAur && !pull && !pushOrSearch) {
			Dotfile::the().list(targets);
		}
	}
	else if (packageOperation) {
		if (addOrAur) {
			Package::the().aurInstall(targets);
		}
		if (install) {
			Package::the().install(targets);
		}
		if (!addOrAur && !install) {
			Package::the().list(targets, pushOrSearch);
		}
	}
	else if (helpOperation) {
		// TODO: open manpage
	}
	else {
		// TODO: open manpage
	}

#ifndef NDEBUG
	printf("%fms\n", t.elapsedNanoseconds() / 1000000.0);
#endif

	return 0;
}
