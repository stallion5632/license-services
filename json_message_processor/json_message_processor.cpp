#include "json_message_processor.h"
#include <boost/property_tree/json_parser.hpp>
#include <exception>
#include <iostream>
#include "utility.hpp"
#include "message_types_res.h"

using namespace boost;
using namespace boost::property_tree;
using namespace std;

JsonMessageProcessor::JsonMessageProcessor()
{
}


JsonMessageProcessor::~JsonMessageProcessor()
{
}

bool JsonMessageProcessor::decodeHeartbeatReq(const std::string& json, HeartbeatReq_t& req)
{
	try
	{
		stringstream ss;
		ss << json;
		read_json(ss, _pt);

		auto data = _pt.get_child_optional("data");
		if (data)
		{
			auto a = data->get_optional<std::string>("ip");
			if (a)
				req.ip = *a;

			auto b = data->get_optional<std::string>("mac");
			if (b)
				req.mac = *b;
			return true;
		}

		return true;
	}
	catch (std::exception &e)
	{
		std::cerr << "[E] decodeHeartbeatReq: unknow error: " << e.what() << endl;
	}
	return false;
}

bool JsonMessageProcessor::decodeHeartbeatRsp(const std::string& json, HeartbeatRsp_t& rsp)
{
	try
	{
		stringstream ss;
		ss << json;
		read_json(ss, _pt);

		auto data = _pt.get_child_optional("data");
		if (data)
		{
			auto a = data->get_optional<int>("code");
			if (a)
				rsp.code = *a;

			auto b = data->get_optional<std::string>("message");
			if (b)
				rsp.message = *b;
			return true;
		}

		return true;
	}
	catch (std::exception &e)
	{
		std::cerr << "[E] decodeHeartbeatRsp: unknow error: " << e.what() << endl;
	}
	return false;
}


bool JsonMessageProcessor::decodeLoginReq(const std::string& json, LoginReq_t& req)
{
	 try
	 {
		stringstream ss;
		ss << json;

		auto a = _pt.get_optional<std::string>("data.login_req");
		if (a)
			req.ip = *a;

		read_json(ss, _pt);
		return true;
	}
	catch (std::exception &e)
	{
		std::cerr << "[E] decodeLoginReq: unknow error: " << e.what() << endl;
	}
	return false;
}

bool JsonMessageProcessor::decodeLoginRsp(const std::string& json, LoginRsp_t& rsp)
{
	try
	{
		stringstream ss;
		ss << json;
		read_json(ss, _pt);

		auto data = _pt.get_child_optional("data");
		if (data)
		{
			auto a = data->get_optional<int>("code");
			if (a)
				rsp.code = *a;

			auto b = data->get_optional<int>("dog_status");
			if (b)
				rsp.dogStatus = *b;

			auto c = data->get_optional<std::string>("message");
			if (c)
				rsp.message = *c;

			return true;
		}
	
		return true;
	}
	catch (std::exception &e)
	{
		std::cerr << "[E] decodeLoginRsp: unknow error: " << e.what() << endl;
	}
	return false;
}

bool JsonMessageProcessor::decodeDogStatusReq(const std::string& json, DogStatusReq_t& req)
{
	return decodeLoginReq(json, req);
}

bool JsonMessageProcessor::decodeDogStatusRsp(const std::string& json, DogStatusRsp_t& rsp)
{
	try
	{
		stringstream ss;
		ss << json;
		read_json(ss, _pt);

		auto data = _pt.get_child_optional("data");
		if (data)
		{
			auto a = data->get_optional<int>("code");
			if (a)
				rsp.code = *a;

			auto b = data->get_optional<int>("dog_status");
			if (b)
				rsp.dogStatus = *b;

			auto c = data->get_optional<std::string>("message");
			if (c)
				rsp.message = *c;

			return true;
		}
		
		return true;
	}
	catch (std::exception &e)
	{
		std::cerr << "[E] decodeDogStatusRsp: unknow error: " << e.what() << endl;
	}
	return false;
}

bool JsonMessageProcessor::decodeDogStatusNtf(const std::string& json, DogStatusNtf_t& ntf)
{
	try
	{
		stringstream ss;
		ss << json;
		read_json(ss, _pt);

		auto data = _pt.get_child_optional("data");
		if (data)
		{
			auto a = data->get_optional<int>("dog_status");
			if (a)
				ntf.dogStatus = *a;

			auto b = data->get_optional<std::string>("message");
			if (b)
				ntf.message = *b;

			return true;
		}
	}
	catch (std::exception &e)
	{
		std::cerr << "[E] decodeDogStatusNtf: unknow error: " << e.what() << endl;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////

std::string JsonMessageProcessor::encodeHeartbeatReq(const HeartbeatReq_t& req)
{
	ptree pt, data;

	pt.put("timestamp", Utility::getTimeStamp());
	data.put("ip", req.ip);
	data.put("mac", req.mac);

	pt.add_child("data", data);

	ostringstream ss;
	write_json(ss, pt);
	return ss.str();
}

std::string JsonMessageProcessor::encodeHeartbeatRsp(const HeartbeatRsp_t& rsp)
{
	ptree pt, data;

	pt.put("timestamp", Utility::getTimeStamp());
	data.put("code", to_string(rsp.code));
	data.put("message", rsp.message);

	pt.add_child("data", data);

	ostringstream ss;
	write_json(ss, pt);
	return ss.str();
}

std::string JsonMessageProcessor::encodeLoginReq(const LoginReq_t& req)
{
	ptree pt, data;

	pt.put("timestamp", Utility::getTimeStamp());
	data.put("ip", req.ip);
	data.put("mac", req.mac);

	pt.add_child("data", data);

	ostringstream ss;
	write_json(ss, pt);
	return ss.str();
}

std::string JsonMessageProcessor::encodeLoginRsp(const LoginRsp_t& rsp)
{
	ptree pt, data;

	pt.put("timestamp", Utility::getTimeStamp());
	data.put("code", to_string(rsp.code));
	data.put("dog_status", to_string(rsp.dogStatus));
	data.put("message", rsp.message);

	pt.add_child("data", data);

	ostringstream ss;
	write_json(ss, pt);
	return ss.str();
}

std::string JsonMessageProcessor::encodeLogoutReq(const LogoutReq_t& req)
{
	ptree pt, data;

	pt.put("timestamp", Utility::getTimeStamp());
	data.put("ip", req.ip);
	data.put("mac", req.mac);

	pt.add_child("data", data);

	ostringstream ss;
	write_json(ss, pt);
	return ss.str();
}

std::string JsonMessageProcessor::encodeLogoutRsp(const LogoutRsp_t& rsp)
{
	ptree pt, data;

	pt.put("timestamp", Utility::getTimeStamp());
	data.put("code", to_string(rsp.code));
	data.put("dog_status", to_string(rsp.dogStatus));
	data.put("message", rsp.message);

	pt.add_child("data", data);

	ostringstream ss;
	write_json(ss, pt);
	return ss.str();
}

std::string JsonMessageProcessor::encodeDogStatusReq(const DogStatusReq_t& req)
{
	ptree pt, data;

	pt.put("timestamp", Utility::getTimeStamp());
	data.put("ip", req.ip);
	data.put("mac", req.mac);

	pt.add_child("data", data);

	ostringstream ss;
	write_json(ss, pt);
	return ss.str();
}

std::string JsonMessageProcessor::encodeDogStatusRsp(const DogStatusRsp_t& rsp)
{
	ptree pt, data;

	pt.put("timestamp", Utility::getTimeStamp());
	data.put("code", to_string(rsp.code));
	data.put("dog_status", to_string(rsp.dogStatus));
	data.put("message", rsp.message);

	pt.add_child("data", data);

	ostringstream ss;
	write_json(ss, pt);
	return ss.str();
}
std::string JsonMessageProcessor::encodeDogStatusNtf(const DogStatusNtf_t& rsp)
{
	ptree pt, data;

	pt.put("timestamp", Utility::getTimeStamp());
	data.put("dog_status", to_string(rsp.dogStatus));
	data.put("message", rsp.message);

	pt.add_child("data", data);

	ostringstream ss;
	write_json(ss, pt);
	return ss.str();
}
