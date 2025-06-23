#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <saura/core/os/os.hpp>
#include <saura/core/utils/utils.hpp>

namespace fs = std::filesystem;
using json	 = nlohmann::json;

TEST_CASE ("config_path")
{
	fs::path home_config_path = Saura::OS::get_home_config_path ();
	home_config_path		  = Saura::Utils::normalize_path (home_config_path);

	fs::path app_path	= home_config_path / "SauraStudios/Notes";
	fs::path app_config = app_path / "config.json";

	std::cout << home_config_path << std::endl;
	std::cout << app_path << std::endl;
	std::cout << app_config << std::endl;

	auto env_user = Saura::OS::get_user_name ();

	if (Saura::Utils::normalize_path ("$HOME/test") !=
		"/home/" + env_user + "/test")
	{
		throw std::runtime_error ("Error!");
	}
	if (Saura::Utils::normalize_path ("~/test") != "/home/" + env_user + "/test")
	{
		throw std::runtime_error ("Error!");
	}

	// 1) Create
	fs::create_directories (app_path);

	// 2.1) Check
	if (!fs::exists (app_path))
	{
		throw std::runtime_error ("Failed create dir");
	}
	// 2.2) Check2
	if (!fs::is_directory (app_path))
	{
		throw std::runtime_error ("Is not dir!");
	}

	// 3) Open
	std::ofstream file (app_config);
	if (!file.is_open ())
	{
		throw std::runtime_error ("Failed open file!");
	}

	// 3.1) Write
	json config;
	config["vaults"][home_config_path.string ()] = {
		{"ts", "1749298616267"},
		{"open", false},
	};
	file << config.dump () << std::endl;
	file.close ();
	std::cout << "File created successfully!" << std::endl;
}
