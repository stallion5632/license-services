#pragma once
#include <string>
#include "tcp_message.hpp"

class Subscriber
{
public:
	virtual ~Subscriber() {}
	virtual void write(TcpMessage& msg) = 0;
};
