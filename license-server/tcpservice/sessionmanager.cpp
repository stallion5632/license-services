#include "subscriber.h"
#include "sessionmanager.h"
#include <functional>
#include <algorithm>

void SessionManager::join(subscriber_ptr subscriber)
{
	_subscribers.insert(subscriber);
}

void SessionManager::leave(subscriber_ptr subscriber)
{
	_subscribers.erase(subscriber);
}

void SessionManager::broadcast(TcpMessage& msg)
{
	std::for_each(_subscribers.begin(), _subscribers.end(),
		std::bind(&Subscriber::write, std::placeholders::_1, std::ref(msg)));
}
