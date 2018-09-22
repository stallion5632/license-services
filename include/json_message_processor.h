#pragma once
#include <string>
#include <boost/property_tree/ptree.hpp>
#include "json_message_processor.h"
#include "message_def.h"
#include "rockapi.h"

// xml message manager
class JsonMessageProcessor
{
public:
	JsonMessageProcessor();
	~JsonMessageProcessor();

	bool decodeHeartbeatReq(const std::string& json, HeartbeatReq_t& req);
	bool decodeHeartbeatRsp(const std::string& json, HeartbeatRsp_t& rsp);

	bool decodeLoginReq(const std::string& json, LoginReq_t& req);
	bool decodeLoginRsp(const std::string& json, LoginRsp_t& rsp);

	bool decodeDogStatusReq(const std::string& json, DogStatusReq_t& req);
	bool decodeDogStatusRsp(const std::string& json, DogStatusRsp_t& rsp);

	bool decodeDogStatusNtf(const std::string& json, DogStatusNtf_t& ntf);

	std::string encodeHeartbeatReq(const HeartbeatReq_t& req);
	std::string encodeHeartbeatRsp(const HeartbeatRsp_t& rsp);

	std::string encodeLoginReq(const LoginReq_t& req);
	std::string encodeLoginRsp(const LoginRsp_t& rsp);

	std::string encodeLogoutReq(const LogoutReq_t& req);
	std::string encodeLogoutRsp(const LogoutRsp_t& rsp);

	std::string encodeDogStatusReq(const DogStatusReq_t& req);
	std::string encodeDogStatusRsp(const DogStatusRsp_t& rsp);

	std::string encodeDogStatusNtf(const DogStatusNtf_t& rsp);

private:
	boost::property_tree::ptree _pt;
};

