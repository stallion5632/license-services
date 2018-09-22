
#include "tcpclient.h"
#include "tcpclient_impl.h"
#include <iostream>

TcpClient::TcpClient(boost::asio::io_context& io_context,
	const ClientConfig& cfg,
	const MessageType_t& messageTypes)
	:_pImpl(new TcpClientImpl(io_context, cfg, messageTypes))
{
}

TcpClient::~TcpClient()
{
	delete _pImpl;
}

void TcpClient::start()
{
    return _pImpl->start();
}

void TcpClient::write(TcpMessage& msg)
{
	return _pImpl->write(msg);
}

void TcpClient::close()
{
	return _pImpl->close();
}

MessageBus<>& TcpClient::messagebus()
{
	return _pImpl->messagebus();
}