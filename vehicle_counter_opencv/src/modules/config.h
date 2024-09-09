#pragma once

#include <unordered_map>
#include <string>

class Config {
private:
	std::unordered_map<std::string, std::string> configMap;
	int movementThreshold;
	std::string localVideo;
	std::vector<int> lane1DectionBox;
public:
	// Constructor that takes a filename
	Config(const std::string& filename);
	int getMovementThreshold() const;
	std::string getLocalVideo() const;
	std::vector<int> getLane1DetectionBox() const;

	// Function to load the configuration file
	bool loadConfig(const std::string& filename);

	// Template function to get a value with a default if not found
	template<typename T>
	T get(const std::string& key, T defaultValue) const;
	Config();

	~Config();
};

/**
 * @brief Retrieves a value from the configuration map.
 *
 * This template method works with types that std::stringstream can handle,
 * such as int or double. If the key is not found in the map, it returns
 * the provided default value.
 *
 * @tparam T Type of the value to retrieve.
 * @param key The key to search for in the configuration map.
 * @param defaultValue The default value to return if the key is not found.
 * @return The value corresponding to the key, or defaultValue if not found.
 */
template<typename T>
T Config::get(const std::string& key, T defaultValue) const {
	if (configMap.find(key) != configMap.end()) {
		std::stringstream ss(configMap.at(key));
		T value;
		ss >> value;
		return value;
	}
	return defaultValue;
}

