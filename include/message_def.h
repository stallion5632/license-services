#pragma once
//
// <<BAS系统升级说明文档.docx>>的定义
//
#include <string>
#include <vector>


struct HeartbeatReq_t
{
	std::string ip;
	std::string mac;
};

struct HeartbeatRsp_t
{
	int code;
	std::string message;
};

// 登陆请求
struct LoginReq_t
{
	std::string ip;		
	std::string mac;			
};

struct LoginRsp_t
{
	int code;
	int dogStatus;
	std::string message;
};

struct DogStatusNtf_t
{
	int dogStatus;
	std::string message;
};

struct LogoutReq_t
{
	std::string ip;
	std::string mac;
};

using DogStatusRsp_t = LoginRsp_t;

using LogoutRsp_t =  LoginRsp_t;

using DogStatusReq_t = LoginReq_t;
