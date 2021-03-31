/*
This program has been developed by students from the bachelor Computer Science at
Utrecht University within the Software Project course.
� Copyright Utrecht University (Department of Information and Computing Sciences)
*/


#include "Flags.h"

std::map<std::string, std::string> Flags::shorthandFlagToLong =
{
	{"v", "version"},
	{"V", "verbose"},
	{"c", "cpu"},
	{"r", "ram"},
	{"o", "output"},
	{"s", "save"}
};

Flags::Flags()
{
	this->mandatoryArgument = "";
	this->flag_cpu = 2;
	this->flag_ram = 8;
	this->flag_output = "console";
	this->flag_save = false;
	this->flag_verbose = utils::VerbosityLevel::All;
}

void Flags::mapShortFlagToLong(std::map<std::string, std::string>& flargs)
{
	std::map<std::string, std::string>::iterator it;

	for (it = flargs.begin(); it != flargs.end(); ++it)
	{
		std::string key = it->first;
		if (Flags::isShortHandFlag(key))
		{
			std::map<std::string, std::string>::iterator lookup = flargs.find(key);
			if (lookup != flargs.end())
			{
				std::string value = lookup->second;
				flargs.erase(lookup);

				flargs[Flags::shorthandFlagToLong[key]] = value;
			}
		}
	}
}

bool Flags::isFlag(std::string flag)
{
	return Flags::isShortHandFlag(flag) || Flags::isLongFlag(flag);
}

bool Flags::isShortHandFlag(std::string flag)
{
	return Flags::shorthandFlagToLong.count(flag) != 0;
}

bool Flags::isLongFlag(std::string flag)
{
	std::map<std::string, std::string> m = Flags::shorthandFlagToLong;
	std::map<std::string, std::string>::iterator it;

	for (it = m.begin(); it != m.end(); ++it)
		if (it->second == flag) return true;

	return false;
}