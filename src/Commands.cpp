/*
This program has been developed by students from the bachelor Computer Science at
Utrecht University within the Software Project course.
� Copyright Utrecht University (Department of Information and Computing Sciences)
*/

#include "Commands.h"
#include <iostream>
#include "Print.h"
#include "Parser2.h"
#include "RunSpider.h"
#include "Utils.h"
#include "Parser.h"
#include "DatabaseRequests.h"
#include "Flags.h"

void Commands::execute(std::string command, Flags flags) 
{

	if (flags.flag_help) {
		help(command);
	}
	else if (flags.flag_version) {
		version(flags);
	}
	else {
		perform[command](flags);
	}
}

bool Commands::isCommand(std::string s) 
{
	return perform.count(s) >= 1;
}

// Commands.

void Commands::start(Flags flags)
{
	auto msg = "Starting a worker node with "
		+ std::to_string(flags.flag_cpu) + " cpu cores and "
		+ std::to_string(flags.flag_ram) + "GB RAM";
	print::log(msg, __FILE__, __LINE__);

	error::err_not_implemented("start", __FILE__, __LINE__);
}

void Commands::check(Flags flags)
{
	auto msg = "Checking the code from the project at "
		+ flags.mandatoryArgument + " against the SearchSECO database";
	print::log(msg, __FILE__, __LINE__);

	std::string tempLocation = "spiderDownloads";
	Commands::downloadRepository(flags.mandatoryArgument, flags, tempLocation);
	std::vector<HashData> hashes = Commands::parseRepository(tempLocation, flags);
	// Calling the function that will print all the matches for us.
	print::printHashMatches(hashes, DatabaseRequests::findMatches(hashes));
	//TODO: delete temp folder.
}

void Commands::upload(Flags flags)
{
	auto msg = "Uploading the code from the project at "
		+ flags.mandatoryArgument + " to the SearchSECO database";
	print::log(msg, __FILE__, __LINE__);

	std::string tempLocation = "spiderDownloads";
	AuthorData authorData = Commands::downloadRepository(flags.mandatoryArgument, flags, tempLocation);
	std::vector<HashData> hashes = Commands::parseRepository(tempLocation, flags);
	// Uploading the hashes.
	ProjectMetaData meta = utils::getProjectMetadata(flags.mandatoryArgument);
	print::printline(DatabaseRequests::uploadHashes(hashes, meta, authorData));
}

void Commands::checkupload(Flags flags)
{
	auto msg = "Checking the code from the project at "
		+ flags.mandatoryArgument + " against the SearchSECO database";
	print::log(msg, __FILE__, __LINE__);

	std::string tempLocation = "spiderDownloads";
	AuthorData authorData = Commands::downloadRepository(flags.mandatoryArgument, flags, tempLocation);
	std::vector<HashData> hashes = Commands::parseRepository(tempLocation, flags);

	ProjectMetaData metaData = utils::getProjectMetadata(flags.mandatoryArgument);
	// Uploading the hashes.
	msg = "Uploading the code from the project at "
		+ flags.mandatoryArgument + " to the SearchSECO database";
	print::log(msg, __FILE__, __LINE__);

	print::printHashMatches(hashes, DatabaseRequests::checkUploadHashes(hashes, metaData, authorData));
}

void Commands::update(Flags flags)
{
	auto msg = "Attempting to update searchseco to version "
		+ flags.mandatoryArgument;
	print::log(msg, __FILE__, __LINE__);

	error::err_not_implemented("update", __FILE__, __LINE__);
}

void Commands::version(Flags flags)
{
	print::version_full();
}

void Commands::help(std::string command)
{
	print::printline("For more information about what each command does, check the documentation.");
	if (command == "" || helpMessagesCommands.count(command) == 0)
	{
		print::printline("\nCommands: \n");
		for (std::string cmd : commandNames)
		{
			print::printline(helpMessagesCommands[cmd]);
			print::printline("");
		}
	}
	else
	{
		print::printline(helpMessagesCommands[command]);
	}
	print::printline("\nCommon flags: \n");
	for (std::string commonFlag : commonFlagNames)
	{
		print::printline(helpMessagesCommonFlags[commonFlag]);
		print::printline("");
	}
}

// Helpers.

AuthorData Commands::downloadRepository(std::string repository, Flags flags, std::string downloadPath)
{
	print::debug("Calling the spider to download a repository", __FILE__, __LINE__);
	return RunSpider::runSpider(repository, downloadPath);
}

std::vector<HashData> Commands::parseRepository(std::string repository, Flags flags)
{
	print::debug("Calling the parser to parse a repository", __FILE__, __LINE__);
	// TODO: fix this somewhere else.
	auto hashes = Parser::parse(repository, flags.flag_cpu);
	for (int i = 0; i < hashes.size(); i++)
	{
		utils::replace(hashes[i].fileName, '/', '\\');
	}
	return hashes;
}

std::map<std::string, std::function<void(Flags)>> Commands::perform =
{
	{"start", start},
	{"check", check},
	{"upload", upload},
	{"checkupload", checkupload},
	{"update", update},
};

std::map<std::string, std::string> Commands::helpMessagesCommands =
{
	{"start", R"(start: Starts a workernode.
	Arguments:
		Storage size (in GB)
		Location where the local database is stored.
	Optionals: 
		-c --cpu: Number of threads used (minimum 2, default half)
		-r --ram: RAM cap (in GB) (default no cap))"},
	{"check", R"(check: Takes a github url, downloads it and parses it. After that it will check for matches with the database.  
	Arguments:
		Url to a github repository.
	Optionals:
		-o --output: Console to print to the console, else you can give a file path.  
		-s --save: Save the parser results for later use.)"},
	{"upload", R"(upload: Takes a github url, downloads it and parses it. Results are send to the database.
	Arguments:
		Url to a github repository.  
	Optionals:
		-s --save: Save the parser results for later use.)"},
	{"checkupload", R"(checkupload: Takes a github url, and does both the check and upload.
	Arguments:
		Url to a github repository.
	Optionals:
		-o --output: Console to print to the console, else you can give a file path.  
		-s --save: Save the parser results for later use.)"},
	{"update", R"(update: Updates the program.
	Optionals:
		The version you want to update to. If no version is specified, the most recent version will be used.)"},
};

std::vector<std::string> Commands::commandNames =
{
	"start",
	"check",
	"upload",
	"checkupload",
	"update",
};

std::map<std::string, std::string> Commands::helpMessagesCommonFlags =
{
	{"verbose", R"(-V --verbose: Sets the verbosity level of the console output. Can be used in combination with any command.
	Arguments:
		The verbosity level. Allowed values are:
			1: Nothing 
			2: Only errors
			3: Warning and errors
			4: Everything (default value))"},
	{"version", R"(-v --version: Displays the version of the SearchSECO system, and the version of each of its subsystems.)"},
	{"help", "-h --help: Displays the help message. Can be used in conjunction with other flags to specify the message."},
};

std::vector<std::string> Commands::commonFlagNames =
{
	"verbose",
	"version",
	"help",
};