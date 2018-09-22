#pragma once

#include <map>
#include "tcp_message.hpp"
#include "rockapi.h"
#include <atomic>
#include <functional>

using MessageType_t = std::map<std::string, int>;
class TcpServerSession;
class MessageHandler
{
public:

	MessageHandler(const MessageType_t& messageTypes, TcpServerSession* session);
	~MessageHandler();
	void readHeader(const TcpMessage& msg);
	void readBody(const TcpMessage& msg);
	void releaseLicense();
	void start();

	static std::atomic<int> s_TotalLicenseNum;

private:
	MessageHandler() = delete;

	void onHeartReq(const TcpMessage& msg);
	void onLoginReq(const TcpMessage& msg);
	void onLogoutReq(const TcpMessage& msg);
	void onGetDogStatusReq(const TcpMessage& msg);
	
	void RegeditUpdateSessionStatus();

	MessageType_t _messageTypes;
	TcpServerSession* _session;
};