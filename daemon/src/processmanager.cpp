#include "ProcessManager.h"
#include "processhelper.hpp"
#include "timer_later.hpp"

ProcessManager::ProcessManager(Config_t& cfg)
	: _cfg(cfg),
	_shmChker(cfg.sharedMemoryName, cfg.totalSharedMemorySize)
{
}

void ProcessManager::startCheckTimer()
{
	later checker(_cfg.checkProcessInterval * 1000, false, &ProcessManager::checkProcessStatus, this);
}

bool ProcessManager::startProcess()
{
	auto tup = ProcessHelper::getAllProcessPidsByName(_cfg.processName);
	auto pids = std::get<0>(tup);
	for (auto pid : pids)
		ProcessHelper::killProcess(pid);

	auto newtup = ProcessHelper::startProcess(_cfg.processName);

	_pid = std::get<0>(newtup);
	if (_pid <= 0)
	{
		std::cerr << "start process error: " << std::get<1>(tup) << "\n";
		return false;
	}
	return true;
}

void ProcessManager::checkProcessStatus()
{
	if (-1 == ProcessHelper::isRunning(_pid))
	{
		std::cerr << "process " << _pid << " is not running! start process "
			<< _cfg.processName << " again£¡" << std::endl;
		startProcess();
	}

#ifdef BOOST_POSIX_API
	else if (-2 == ProcessHelper::isRunning(_pid))
	{
		std::cerr << "process " << _pid << " is zombie!" << std::endl;
		if (waitpid(_pid, NULL, 0) != _pid)
		{
			std::cerr << "terminate zombie process " << _pid << " failed!" << std::endl;
		}
		startProcess();
	}
#endif

	if (_cfg.sharedMemoryCheck && -2 == _shmChker.check())
	{
		std::cerr << "process " << _pid << " sharedmeory check failed! start process "
			<< _cfg.processName << " again£¡" << std::endl;
		startProcess();
	}
	later checker(_cfg.checkProcessInterval * 1000, false, &ProcessManager::checkProcessStatus, this);
}
