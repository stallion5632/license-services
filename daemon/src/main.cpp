#include "processhelper.hpp"
#include <iostream>
#include "appconfig_reader.h"
#include "processmanager.h"

int main(int argc, char *argv[])
{
	try
	{
		// loadAppConfig
		const std::string cfgFilePath = boost::filesystem::current_path().string() + std::string("/daemonapp.cfg");
		Config_t cfg;
		if (boost::filesystem::exists(cfgFilePath))
			cfg = std::move(loadAppConfig(cfgFilePath));

		ProcessManager pm(cfg);
		pm.startProcess();
        pm.startCheckTimer();

		char c(' ');
		while (c != 'q' && c != 'Q')
		{
			std::cout << "Press q then enter to quit: \n";
			std::cin >> c;
		}
	}
	catch (std::exception &e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}

	return 0;
}
