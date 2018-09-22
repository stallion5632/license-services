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

	// 开始认证服务
	// cfg - 客户端的配置
	// messageTypes - 消息映射表，必须和server保持一致
	// cb - 看门狗状态的回调函数
	void start(ClientConfig& cfg, MessageType_t& messageTypes, std::function<void(int)> cb);

	// 停止认证服务
	void stop();

	void sendHeartbeatReq();
	void sendLoginReq();
	void sendLogoutReq();
	void sendGetDogStatusReq();

private:
	LicenseClientImpl * _pImpl;
};

