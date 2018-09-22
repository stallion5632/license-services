#pragma once

#include <string>

struct Config_t
{
    std::string processName;
    int checkProcessInterval = 3; // second

	// shared memory checker
	bool sharedMemoryCheck = true;
	uint32_t totalSharedMemorySize = 128; // bytes
	std::string sharedMemoryName = "abcd"; 
};

Config_t loadAppConfig(const std::string &filepath);

