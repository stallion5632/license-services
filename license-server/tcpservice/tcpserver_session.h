#pragma once

#include <deque>
#include <boost/asio.hpp>
#include "tcp_message.hpp"
#include <stdint.h>

#include "subscriber.h"
#include "sessionmanager.h"
#include "messagehandler.h"
#include "dogservice.h"
#include "common.h"

using TcpMessageQueue = std::deque<TcpMessage>;
using MessageType_t = std::map<std::string, int>;


class TcpServerSession
	: public Subscriber, public std::enable_shared_from_this<TcpServerSession>
{
public:
	TcpServerSession(boost::asio::io_context &io_context,
		SessionManager& ch,
		MessageType_t &messageTypes, DogService& dogService, ServerConfig& cfg)
		: _sessionManger(ch),
		_io_context(io_context),
		_socket(io_context),
		_heartbeatTimer(io_context),
		_messageTypes(messageTypes),
		_messagehandler(messageTypes, this),
		_dogService(dogService),
		_cfg(cfg)
	{
		_dogService.setTimerInterval(_cfg.dogCheckInterval);
	}

	void write(TcpMessage& msg);
	void start();
	void close();

	boost::asio::ip::tcp::socket& socket()
	{
		return _socket;
	}

	DogService& dogService() { return _dogService; }
	ServerConfig& serverConfig() { return _cfg; }
private:

	void startRead();

	void handleReadHeader(const boost::system::error_code& ec);

	void handleReadBody(const boost::system::error_code& ec);

	void doWrite(TcpMessage& msg);

	void startWrite(const TcpMessage& msg);

	void handleWrite(const boost::system::error_code& ec);

	void checkHeartbeat();

	void doClose();

private:

	bool _stopped = false;
	boost::asio::io_context& _io_context;
	boost::asio::ip::tcp::socket _socket;
	boost::asio::deadline_timer _heartbeatTimer;

	TcpMessage _readMsg;
	TcpMessageQueue _writeMsgs;

	bool _isRecvedHeartbeatReq = false;	// heart beat check flag

	uint32_t _currentSeq = 0;	// always increase
	uint32_t _heartBeatLostTimes = 0;

	std::string _rbuf;		// message bytes read buffer
	std::string _wbuf;		// message bytes write buffer

	MessageType_t& _messageTypes;
	SessionManager& _sessionManger;
	MessageHandler _messagehandler;
	DogService& _dogService;
	ServerConfig _cfg;
};
