
#ifndef TCP_CLIENT_IMPL_H
#define TCP_CLIENT_IMPL_H

#include <deque>
#include <boost/asio.hpp>
#include "tcp_message.hpp"
#include "messagebus.hpp"
#include <stdint.h>
#include "common.h"

using TcpMessageQueue = std::deque<TcpMessage>;

class TcpClientImpl
{
public:
  TcpClientImpl(boost::asio::io_context &io_context,
				const ClientConfig &cfg,
				const MessageType_t &messageTypes)
	  : _io_context(io_context),
		_socket(io_context),
		_heartbeatTimer(io_context),
		_reconnectTimer(io_context),
		_messageTypes(messageTypes),
		_clientConfig(cfg)
  {
	  boost::asio::ip::tcp::resolver resolver(io_context);
	  _endpoints = resolver.resolve(_clientConfig.serverIP, _clientConfig.serverPort);
	}

	void write(TcpMessage& msg);
	
	void close();
	
	MessageBus<>& messagebus() { return _messagebus; }

    void start();
    
private:

	void startConnect();
	
	void handleConnect(const boost::system::error_code& ec);
	
	void doReconnect(const boost::system::error_code& ec);
	
	void startRead();
	
	void handleReadHeader(const boost::system::error_code& ec);
	
	void handleReadBody(const boost::system::error_code& ec);
	
	void doWrite(TcpMessage& msg);
	
	void startWrite(const TcpMessage& msg);
	
	void handleWrite(const boost::system::error_code& ec);
	
	void checkHeartbeat();
	
	void doClose();
	
	void startReconnect();
	
private:

    ClientConfig _clientConfig;
	bool _stopped = false;
	boost::asio::io_context& _io_context;
	boost::asio::ip::tcp::socket _socket;
	boost::asio::deadline_timer _heartbeatTimer;
	boost::asio::deadline_timer _reconnectTimer;

	TcpMessage _readMsg;
	TcpMessageQueue _writeMsgs;

	bool _isRecvedHeartbeatRsp = false;

    uint32_t _currentSeq = 0;	// always increase
	uint32_t _heartBeatLostTimes = 0;

	std::string _rbuf;	// message bytes read buffer
	std::string _wbuf;	// message bytes write buffer

	boost::asio::ip::tcp::resolver::results_type _endpoints;
	MessageType_t _messageTypes;
	MessageBus<> _messagebus;
};

#endif
