#include "Config.h"
#include "Definitions.h"

Config read_ini_file(RawString name)
{
	mINI::INIFile configFile(name);
	Config get_config;

	assert(configFile.read(get_config));
	return get_config;
}

Config load_config_from_ini_file(RawString fileName)
{
	Config get_config{ read_ini_file(fileName) };
	for (auto it : get_config)
	{
		auto const& category = it.first;
		auto const& collection = it.second;
		for (auto const& it2 : collection)
		{
			auto const& token = it2.first;
			auto const& value = it2.second;
			Logger::info("Config loaded: {}.{} = {}", category, token, value);
		}
	}

	return get_config;
}

Config override_config_with_command_line(Config get_config, Size argumentCount, RawString arguments[])
{
	Logger::info("Config loaded: dagger.path = {}", arguments[0]);
	get_config["dagger"]["path"] = arguments[0];

	for (int i = 1; i < argumentCount; i++)
	{
		Size delimiter = 0;
		String flag{ arguments[i] };
		String token = flag;
		String value = "";

		if ((delimiter = flag.find("=")) != String::npos)
		{
			token = flag.substr(0, delimiter);
			flag.erase(0, delimiter + 1);
			value = flag;
		}
		else
		{
			Bool sign = !(flag.at(0) == '~');
			value = sign ? "true" : "false";
			if (sign == false) { flag.erase(0, 1); }
			token = flag;
		}

		String category = "uncategorized";
		if ((delimiter = token.find(".")) != String::npos)
		{
			category = token.substr(0, delimiter);
			token.erase(0, delimiter + 1);
		}

		Logger::info("Config override: {}.{} = {}", category, token, value);
		get_config[category][token] = value;
	}

	return get_config;
}