#include "config.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

Config::Config(const std::string& filename) {
	loadConfig(filename);
	movementThreshold = get<int>("movement_threshold", 5000);
	localVideo = get<std::string>("local_video", std::string("error"));
	lane1DectionBox.push_back(get<int>("lane1_x", 0));
	lane1DectionBox.push_back(get<int>("lane1_y", 0));
	lane1DectionBox.push_back(get<int>("lane1_width", 0));
	lane1DectionBox.push_back(get<int>("lane1_height", 0));
}

Config::Config()
{
}

Config::~Config() {
}

bool Config::loadConfig(const std::string& filename) {
	std::ifstream configFile(filename);
	if (!configFile.is_open()) {
		std::cerr << "Error: Could not open config file." << std::endl;
		return false;
	}

	std::string line;
	while (getline(configFile, line)) {
		if (line.empty() || line[0] == '#') continue;

		size_t delimiterPos = line.find('=');
		if (delimiterPos != std::string::npos) {
			std::string key = line.substr(0, delimiterPos);
			std::string value = line.substr(delimiterPos + 1);
			configMap[key] = value;
		}
	}

	configFile.close();
	return true;
}

int Config::getMovementThreshold() const {
	return movementThreshold;
}
std::string Config::getLocalVideo() const {
	return localVideo;
}
std::vector<int> Config::getLane1DetectionBox() const {
	return lane1DectionBox;
}


 


