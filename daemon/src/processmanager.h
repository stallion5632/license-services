#pragma once
#include "appconfig_reader.h"
#include "shared_memory_checker.hpp"

class ProcessManager
{
public:
	ProcessManager(Config_t& cfg);

	void startCheckTimer();

	bool startProcess();

	void checkProcessStatus();

private:
	int _pid = 0;
	Config_t _cfg;
	SharedMemoryChecker _shmChker;
};
