#pragma once

#include "rockapi.h"
#include "common.h"
#include <functional>

class LicenseClientImpl;
class ROCK_HANDLER_API LicenseClient
{
public:
	LicenseClient();
	~LicenseClient();

	// ��ʼ��֤����
	// cfg - �ͻ��˵�����
	// messageTypes - ��Ϣӳ��������server����һ��
	// cb - ���Ź�״̬�Ļص�����
	void start(ClientConfig& cfg, MessageType_t& messageTypes, std::function<void(int)> cb);

	// ֹͣ��֤����
	void stop();

	void sendHeartbeatReq();
	void sendLoginReq();
	void sendLogoutReq();
	void sendGetDogStatusReq();

private:
	LicenseClientImpl * _pImpl;
};

