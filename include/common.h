#pragma once
#include <stdint.h>
#include <string>
#include <map>

static const uint32_t kIntLen = 4;							// sizeof(int) is 4
static const uint32_t kTotalHeadLen = kIntLen * 8;			// head of TcpMessage is 4*8
static const uint32_t kMaxBodySize = 1024 * 10;				// Max TcpMessage body size 
static const uint32_t kMaxMessageSize = 1024 * 1024 * 64;	// Max TcpMessage size

static const char* kMsgStart = "handleStart";				// first thing todo when connected
static const char* kMsgHeartbeatReq = "sendHeartbeatReq";	// send heartbeat_req 
static const char* kMsgReadHeader = "handleReadHeader";		// MessageBus register topic for sending read header to messagehandler module
static const char* kMsgReadBody = "handleReadBody";			// essageBus register topic for sending read body to messagehandler module

struct ClientConfig
{
	bool isReconnect = false;
	int reconnectInterval = 5;
	int heartbeatInterval = 10;
	std::string serverIP;
	std::string serverPort;
};

struct ServerConfig
{
	int maxSupportClientNum = 5; // 支持的最大license个数
	int dogCheckInterval = 1;	// 加密狗状态检测间隔(秒)
	int heartbeatInterval = 10; // 心跳检测间隔(秒)
	int serverPort = 5555;
};

using MessageType_t = std::map<std::string, int>;