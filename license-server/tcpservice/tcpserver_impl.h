#pragma once
#include <boost/asio.hpp>
#include <set>
#include "sessionmanager.h"
#include "dogservice.h"
#include "common.h"

class TcpServerSession;

typedef std::shared_ptr<TcpServerSession> TcpServerSessionPtr;
using MessageType_t = std::map<std::string, int>;
class TcpServerImpl
{
public:
	TcpServerImpl(boost::asio::io_context& io_context,
		const boost::asio::ip::tcp::endpoint& listen_endpoint,
		MessageType_t& messageType,
		ServerConfig& cfg);
	~TcpServerImpl();

	void startAccept();
	void handleAccept(TcpServerSessionPtr session, const boost::system::error_code& ec);

private:
	boost::asio::io_context& _io_context;
	boost::asio::ip::tcp::acceptor _acceptor;
	SessionManager _sessions;
	MessageType_t& _messageTypes;
	ServerConfig _cfg;
	DogService _dogService;
};

