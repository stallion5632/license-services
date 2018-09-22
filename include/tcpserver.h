#pragma once
#include <boost/asio.hpp>
#include "rockapi.h"
#include "common.h"

class TcpServerImpl;
class ROCK_NET_API TcpServer
{
public:
	TcpServer(boost::asio::io_context& io_context,
		const boost::asio::ip::tcp::endpoint& listen_endpoint,
		std::map<std::string, int>& messageType, ServerConfig& cfg);
	~TcpServer();

private:

	TcpServerImpl * _pImpl;
};

