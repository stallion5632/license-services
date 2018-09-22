#pragma once

#include <map>
#include "tcp_message.hpp"
#include "rockapi.h"
#include <functional>

using MessageType_t = std::map<std::string, int>;
class TcpClient;
class ROCK_HANDLER_API MessageHandler
{
public:

	MessageHandler(const MessageType_t& messageTypes, TcpClient* client);
	void onReadHeader(TcpMessage msg);
	void onReadBody(TcpMessage msg);
	void regeditDogStatusCallback(std::function<void(int)> cb);

private:
	MessageHandler() = delete;
	MessageType_t _messageTypes;
	TcpClient* _client = 0;
	std::function<void(int)> _dogStatusCallback;
};