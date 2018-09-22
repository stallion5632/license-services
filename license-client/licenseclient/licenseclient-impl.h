#pragma once

#include <boost/asio.hpp>
#include "common.h"
#include "tcp_message.hpp"
#include <functional>

class TcpClient;
class LicenseClientImpl
{
public:
	LicenseClientImpl() = default;
	~LicenseClientImpl() = default;

	void start(ClientConfig& cfg, MessageType_t& messageTypes, std::function<void(int)> cb);
	void stop();

	void sendHeartbeatReq();
	void sendLoginReq();
	void sendLogoutReq();
	void sendGetDogStatusReq();

private:
	void onStart();
	void onSendHeartbeatReq();
	void onReadHeader(TcpMessage msg);
	void onReadBody(TcpMessage msg);
	void regeditDogStatusCallback(std::function<void(int)> cb);

	TcpClient* _client = nullptr;
	MessageType_t _messageTypes;
	std::function<void(int)> _dogStatusCallback;
	boost::asio::io_context _io_context;
};

