#include "licenseclient.h"
#include "licenseclient-impl.h"
#include "tcpclient.h"

using namespace boost;

LicenseClient::LicenseClient()
	:_pImpl(new LicenseClientImpl())
{
}

LicenseClient::~LicenseClient()
{
	delete _pImpl;
}

void LicenseClient::start(ClientConfig& cfg, MessageType_t& messageTypes, std::function<void(int)> cb)
{
	_pImpl->start(cfg, messageTypes, cb);
}

void LicenseClient::stop()
{
	_pImpl->stop();
}

void LicenseClient::sendHeartbeatReq()
{
	_pImpl->sendHeartbeatReq();
}

void LicenseClient::sendLoginReq()
{
	_pImpl->sendLoginReq();
}

void LicenseClient::sendLogoutReq()
{
	_pImpl->sendLogoutReq();
}

void LicenseClient::sendGetDogStatusReq()
{
	_pImpl->sendGetDogStatusReq();
}
