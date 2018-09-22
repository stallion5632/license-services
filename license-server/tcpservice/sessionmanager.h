#pragma once
#include <string>
#include <set>
#include "subscriber.h"

typedef std::shared_ptr<Subscriber> subscriber_ptr;

class SessionManager
{
public:
	void join(subscriber_ptr subscriber);

	void leave(subscriber_ptr subscriber);

	void broadcast(TcpMessage& msg); // broadcast to all client

private:
	std::set<subscriber_ptr> _subscribers;
};