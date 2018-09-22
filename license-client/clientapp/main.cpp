
#include "tcpclient.h"
#include <iostream>
#include <thread>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include "common.h"
#include "message_types_res.h"
#include "appconfig_reader.h"
#include "message_type_reader.h"
#include "licenseclient.h"

using namespace boost;
using namespace std;
using boost::asio::ip::tcp;


int menu()
{
	int choice;

	cout << "\n**** Menu **** " << endl;
	cout << "(0) Quit. " << endl;
	cout << "(1) login. " << endl;
	cout << "(2) logout. " << endl;
	cout << "(3) getDogStatus. " << endl;
	cout << ": " << endl;
	cin >> choice;
	return choice;
}

void updateDogStatus(int status)
{
	std::cout << "[main] updateDogStatus status: " << status << std::endl;
}

int main(int argc, char *argv[])
{
	try
	{
		// loadAppConfig
		const std::string cfgFilePath = boost::filesystem::current_path().string() + std::string("/license-client.cfg");
		ClientConfig cfg;
		if (boost::filesystem::exists(cfgFilePath))
			cfg = std::move(loadAppConfig(cfgFilePath));

		if (argc >= 3)
		{
			cfg.serverIP = argv[1];
			cfg.serverPort = argv[2];
		}
		else
		{
			std::cout << "cfg.serverIP: " << cfg.serverIP << ", cfg.serverPort: " << cfg.serverPort << std::endl;
			std::cout << "!!!You can also input <host> <port> on the terminal!!!\n";
		}

		// loadMessageTypes
		const std::string messageTypeFilePath = boost::filesystem::current_path().string() + std::string("/message_types.json");
		MessageType_t messageTypes;
		if (boost::filesystem::exists(messageTypeFilePath))
			messageTypes = loadMessageTypes(messageTypeFilePath);

		if (messageTypes.size() == 0)
		{
			std::cerr << "[E] load message_types.json failed!" << std::endl;
			return -1;
		}
		LicenseClient licenseClient;
		std::thread t([&cfg, &messageTypes, &licenseClient]() { licenseClient.start(cfg, messageTypes, updateDogStatus); });
		t.detach();

		bool exit = false;
		for (;;) {
			_sleep(100);
			int choice = menu();
			switch (choice)
			{
			case (0):
				exit = true;
				break;
			case (1):
			{
				licenseClient.sendLoginReq();
			}
			break;
			case (2):
			{
				licenseClient.sendLogoutReq();
			}
			break;
			case (3):
			{
				licenseClient.sendGetDogStatusReq();
			}
			break;
			default:
				cout << "Please select again! " << endl;
				break;
			}

			if (exit == true)
				break;
		} // end forever
		licenseClient.stop();
		// t.join();
	}
	catch (std::exception &e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}

	return 0;
}
