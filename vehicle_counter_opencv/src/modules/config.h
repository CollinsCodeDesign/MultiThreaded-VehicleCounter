#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

class JsonConfigLoader {
private:
	nlohmann::json configData; // JSON object to store configuration data

	bool keyExists(const std::string& key) const;
public:
	// Constructor that takes a filename
	JsonConfigLoader(const std::string& configFilePath);
	bool loadConfig(const std::string& configFilePath);

	// Method to get a configuration value as a string
	std::string getStringValue(const std::string& key) const;

	// Method to get a configuration value as an integer
	int getIntValue(const std::string& key) const;

	// Method to get a configuration value as a boolean
	bool getBoolValue(const std::string& key) const;

	// Method to get a configuration value as a double
	double getDoubleValue(const std::string& key) const;
	JsonConfigLoader();

	~JsonConfigLoader();
};

