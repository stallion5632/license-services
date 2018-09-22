
#include "tcpserver.h"
#include "tcpserver_impl.h"

TcpServer::TcpServer(boost::asio::io_context& io_context,
	const boost::asio::ip::tcp::endpoint& listen_endpoint, std::map<std::string, int>& messageType, ServerConfig& cfg)
	:_pImpl(new TcpServerImpl(io_context, listen_endpoint, messageType, cfg))
{
}

TcpServer::~TcpServer()
{
	delete _pImpl;
}

