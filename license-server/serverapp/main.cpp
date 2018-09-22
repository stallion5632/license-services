
#include "tcpserver.h"

#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include "message_type_reader.h"
#include "appconfig_reader.h"

using namespace boost;
using boost::asio::ip::tcp;

int main(int argc, char *argv[])
{
	try
	{
		// loadAppConfig
		const std::string cfgFilePath = boost::filesystem::current_path().string() + std::string("/license-server.cfg");
		ServerConfig cfg;
		if (boost::filesystem::exists(cfgFilePath))
			cfg = std::move(loadAppConfig(cfgFilePath));

		if (argc >= 2)
		{
			cfg.serverPort = std::atoi(argv[1]);
		}
		else
		{
			std::cout << "cfg.serverPort: " << cfg.serverPort << std::endl;
			std::cout << "!!!You can also input <port> on the terminal!!!\n";
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

		boost::asio::io_context io_context;

		tcp::endpoint listen_endpoint(tcp::v4(), cfg.serverPort);

		TcpServer s(io_context, listen_endpoint, messageTypes, cfg);

		io_context.run();

	}
	catch (std::exception &e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}

	return 0;
}
