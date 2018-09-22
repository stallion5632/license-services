
#include "tcpserver_impl.h"
#include "tcpserver_session.h"

#include <boost/filesystem.hpp>

using namespace boost;
using boost::asio::ip::tcp;

TcpServerImpl::TcpServerImpl(boost::asio::io_context& io_context,
	const tcp::endpoint& listen_endpoint, MessageType_t &messageTypes, ServerConfig& cfg)
	: _io_context(io_context),
	_acceptor(io_context, listen_endpoint),
	_messageTypes(messageTypes),
	_cfg(cfg)
{
	MessageHandler::s_TotalLicenseNum = _cfg.maxSupportClientNum;
	startAccept();
	_dogService.start();
}

TcpServerImpl::~TcpServerImpl()
{
	_dogService.stop();
}


void TcpServerImpl::startAccept()
{
	TcpServerSessionPtr new_session(new TcpServerSession(_io_context, _sessions, _messageTypes, _dogService, _cfg));

	_acceptor.async_accept(new_session->socket(),
		std::bind(&TcpServerImpl::handleAccept, this, new_session, std::placeholders::_1));
}

void TcpServerImpl::handleAccept(TcpServerSessionPtr session, const boost::system::error_code& ec)
{
	if (!ec)
	{
		session->start();
	}

	startAccept();
}



