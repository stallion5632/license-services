#pragma once
#include <chrono>
#include <stdint.h>
#include <boost/process.hpp>
#include <boost/process/extend.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <string>
#include "utility.hpp"
#if defined(BOOST_WINDOWS_API)
#include <boost/locale.hpp>
#include <TlHelp32.h>

#elif defined(BOOST_POSIX_API)
#include <sys/types.h>  
#include <signal.h>  
#endif

namespace ProcessHelper
{
	static std::string getProcessRunningTime(uint64_t startTimeStamp) // return minutes
	{
		auto now = Utility::getTimeStamp();
		auto diff = now - startTimeStamp;
		auto min = double(diff) / (1000 * 60);
		return std::to_string(min);
	}

#if defined(BOOST_WINDOWS_API)
    static int isRunning(int pid)
	{
		HANDLE pss = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);

		PROCESSENTRY32 pe = { 0 };
		pe.dwSize = sizeof(pe);

		if (Process32First(pss, &pe))
		{
			do
			{
				// pe.szExeFile can also be useful  
				if (pe.th32ProcessID == pid)
                    return 0;
			} while (Process32Next(pss, &pe));
		}

		CloseHandle(pss);

        return -1;
	}

#elif defined (BOOST_POSIX_API)

    // 0 : running, -1 : exit, -2 : zombie
    static int isRunning(int pid)
	{
        if ( 0 == kill(pid, 0))
        {
           std::string path = std::string("/proc/") + std::to_string(pid) + "/status" ;
           std::ifstream fs;
           fs.open(path);
           if (!fs)
                return -1;

           std::string line;
           while(getline(fs, line))
           {
               std::size_t found = line.find("State:");
                if (found==std::string::npos)
                    continue;
                else
                {
                    found = line.find("zombie");
                    if (found == std::string::npos)
                        return 0;
                    else
                        return -2;  // zombie
                }
           }
        }
        else
            return -1;
	}
#endif


	static std::tuple<bool, std::string> killProcess(int pid)
	{
		std::string err;
		bool ret = false;
		try
		{
#if defined(BOOST_WINDOWS_API)
		auto id = boost::process::pid_t(pid);
		boost::process::child pros(id);
		std::error_code ec;
		pros.terminate(ec);

		if (ec.value() == 0)
			return std::make_tuple(true, err);

#elif defined(BOOST_POSIX_API)  // cannot kill the child process correctly
		std::string cmd("kill ");
		cmd += std::to_string(pid);
		auto ret = boost::process::system(cmd);
		if (ret == 0)
			return std::make_tuple(true, err);
#endif
		}
		catch (boost::process::process_error& exc)
		{
			err = exc.what();
		}
		return std::make_tuple(false, err);
	}

	static std::tuple<std::vector<int>, std::string> getAllProcessPidsByName(const std::string& processName)
	{
		std::vector<int> pids;
		std::string err;
		try
		{
#if defined(BOOST_WINDOWS_API)

			std::wstring wName;

			std::string exe(".exe");

			wName = boost::locale::conv::to_utf<wchar_t>(processName + exe, "GBK");

			HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

			PROCESSENTRY32W entry;
			entry.dwSize = sizeof entry;
			if (!Process32FirstW(hSnapShot, &entry))
			{
				return std::make_tuple(std::move(pids), err);
			}
			do {
				if (std::wstring(entry.szExeFile) == wName)
				{
					pids.emplace_back(entry.th32ProcessID);
				}
			} while (Process32NextW(hSnapShot, &entry));


#elif defined (BOOST_POSIX_API)
			boost::filesystem::path path = "/proc";
			boost::filesystem::directory_iterator end;

			for (boost::filesystem::directory_iterator iter(path); iter != end; iter++)
			{
				boost::filesystem::path p = *iter;
                std::ifstream statusFile;
                statusFile.open(p.string() + std::string("/status"));
				if (!statusFile)
					continue;

                std::string statusContent;
				getline(statusFile, statusContent);
                std::vector<std::string> a;
				boost::algorithm::split(a, statusContent, boost::algorithm::is_any_of(":"), boost::algorithm::token_compress_on);

				if (boost::algorithm::trim_copy(a[1]) == processName)
				{
					pids.push_back(std::stoi(p.leaf().string()));
				}
				statusFile.close();
				statusFile.clear();
			}
#endif
		}
		catch (boost::process::process_error& exc)
		{
			err = exc.what();
		}
		return std::make_tuple(std::move(pids), err);
	}


	static std::tuple<int, std::string> startProcess(const std::string& processName)
	{
		int pid = -1;
		std::string err;

		try
		{
#if defined(BOOST_WINDOWS_API)
			auto p = processName + ".exe";
			if (!boost::filesystem::exists(p))
			{
				p = boost::filesystem::current_path().string() + "/" + p;
				if (!boost::filesystem::exists(p))
				{
					err = "process not exist";
					return std::make_tuple(pid, err);
				}
			}
			boost::process::child c(p, boost::process::extend::on_setup = [](auto &exec) {
				exec.creation_flags |= boost::winapi::CREATE_NEW_CONSOLE_;
			});

#elif defined(BOOST_POSIX_API)

			auto p = processName;
			if (!boost::filesystem::exists(p))
			{
				p = boost::filesystem::current_path().string() + "/" + p;
				if (!boost::filesystem::exists(p))
				{
					err = "process not exist";
					return std::make_tuple(pid, err);
				}
			}
			boost::process::child c(p);
#endif
			pid = c.id();
			// detach as a single process
			c.detach();
		}
		catch (boost::process::process_error& exc)
		{
			err = exc.what();
			pid = -1;
		}
		return std::make_tuple(pid, err);
	}
}
