#include "config.h"
#include <iostream>
#include <fstream>

JsonConfigLoader::JsonConfigLoader(const std::string& configFilePath) {
	if (!loadConfig(configFilePath)) {
		std::cerr << "Failed to load configuration file: " << configFilePath << std::endl;
	}
}

JsonConfigLoader::JsonConfigLoader()
{
}

JsonConfigLoader::~JsonConfigLoader() {
}

bool JsonConfigLoader::loadConfig(const std::string& configFilePath) {
	std::ifstream configFile(configFilePath);

	if (!configFile.is_open()) {
		std::cerr << "Could not open config file: " << configFilePath << std::endl;
		return false;
	}

	try {
		configFile >> configData;
	}
	catch (nlohmann::json::parse_error& e) {
		std::cerr << "JSON parsing error: " << e.what() << std::endl;
		return false;
	}

	return true;
}

// Get a string value from the configuration
std::string JsonConfigLoader::getStringValue(const std::string& key) const {
	if (keyExists(key)) {
		return configData[key].get<std::string>();
	}
	return "";
}

// Get an integer value from the configuration
int JsonConfigLoader::getIntValue(const std::string& key) const {
	if (keyExists(key)) {
		return configData[key].get<int>();
	}
	return 0;
}

// Get a boolean value from the configuration
bool JsonConfigLoader::getBoolValue(const std::string& key) const {
	if (keyExists(key)) {
		return configData[key].get<bool>();
	}
	return false;
}

// Get a double value from the configuration
double JsonConfigLoader::getDoubleValue(const std::string& key) const {
	if (keyExists(key)) {
		return configData[key].get<double>();
	}
	return 0.0;
}

// Check if a key exists in the configuration
bool JsonConfigLoader::keyExists(const std::string& key) const {
	return configData.contains(key);
}


 


