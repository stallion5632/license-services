#include "messagehandler.h"
#include "tcpserver_session.h"
#include "tcp_message.hpp"
#include <iostream>
#include "common.h"
#include "loghelper.h"
#include "message_types_res.h"
#include "json_message_processor.h"

using namespace RockLog;

std::atomic<int> MessageHandler::s_TotalLicenseNum = 0;
static JsonMessageProcessor s_jsonMessageProcessor;

MessageHandler::MessageHandler(const MessageType_t &messageTypes, TcpServerSession* session)
	: _messageTypes(messageTypes),
	  _session(session)
{
}

void MessageHandler::start()
{
	RegeditUpdateSessionStatus();
}

void MessageHandler::RegeditUpdateSessionStatus()
{
	std::function<void(int)> cb = [this](int status)
	{
		std::cout << "dog status: " << status << std::endl;
		if (status != 0)
		{
			std::cout << "dog status error! status: " << status << std::endl;

			TcpMessage m;
			DogStatusNtf_t ntf;
			ntf.dogStatus = status;
			m.body = s_jsonMessageProcessor.encodeDogStatusNtf(ntf);
			m.msgType = _messageTypes[kDogStatusNtf];
			_session->write(m);
			// this->_sessionManger->close();
		}
	};
	_session->dogService().updateSessionStatus(cb);
}

void MessageHandler::releaseLicense()
{
	s_TotalLicenseNum += 1;
}

MessageHandler::~MessageHandler()
{
	_session->dogService().stop();
}

void MessageHandler::readHeader(const TcpMessage& msg)
{
	if (_messageTypes[kHeartReq] == msg.msgType)
	{
		LOG(kInfo) << "_messageTypes: " << kHeartReq << std::endl;
	}
	else if (_messageTypes[kLoginReq] == msg.msgType)
	{
		LOG(kInfo) << "_messageTypes: " << kLoginReq << std::endl;
	}
	else if (_messageTypes[kLogoutReq] == msg.msgType)
	{
		LOG(kInfo) << "_messageTypes: " << kLogoutReq << std::endl;
	}
	else if (_messageTypes[kGetDogStatusReq] == msg.msgType)
	{
		LOG(kInfo) << "_messageTypes: " << kGetDogStatusReq << std::endl;
	}
}

void MessageHandler::readBody(const TcpMessage& msg)
{
	LOG(kInfo) << "_session: " << _session << std::endl
		<< "msg.msgType: " << msg.msgType << std::endl
		<< "msg.body: " << msg.body << std::endl;

	if (_messageTypes[kHeartReq] == msg.msgType)
	{
		onHeartReq(msg);
	}
	else if (_messageTypes[kLoginReq] == msg.msgType) 
	{
		onLoginReq(msg);
	}
	else if (_messageTypes[kLogoutReq] == msg.msgType) 
	{
		onLogoutReq(msg);
	}
	else if (_messageTypes[kGetDogStatusReq] == msg.msgType)
	{
		onGetDogStatusReq(msg);
	}
}

void MessageHandler::onHeartReq(const TcpMessage& msg)
{
	TcpMessage m;
	m.syncBytes = msg.syncBytes;

	HeartbeatRsp_t rsp;
	rsp.code = 1000;
	m.body = s_jsonMessageProcessor.encodeHeartbeatRsp(rsp);

	m.msgType = _messageTypes[kHeartRsp];
	_session->write(m);
}


void MessageHandler::onLoginReq(const TcpMessage& msg)
{
	TcpMessage m;
	m.syncBytes = msg.syncBytes;

	if (s_TotalLicenseNum > 0)
	{
		LoginRsp_t rsp;
		rsp.code = 1000;
		rsp.dogStatus = _session->dogService().getCurrentStatus();
		m.body = s_jsonMessageProcessor.encodeLoginRsp(rsp);
		s_TotalLicenseNum -= 1;
	}
	else
	{
		LoginRsp_t rsp;
		rsp.code = 1002;
		rsp.dogStatus = _session->dogService().getCurrentStatus();
		m.body = s_jsonMessageProcessor.encodeLoginRsp(rsp);	// License num over limit
	}

	m.msgType = _messageTypes[kLoginRsp];
	_session->write(m);
}

void MessageHandler::onLogoutReq(const TcpMessage& msg)
{
	TcpMessage m;
	LogoutRsp_t rsp;
	rsp.code = 1000;
	rsp.dogStatus = _session->dogService().getCurrentStatus();
	m.body = s_jsonMessageProcessor.encodeLogoutRsp(rsp);
	m.msgType = _messageTypes[kLogoutRsp];
	_session->write(m);

	s_TotalLicenseNum += 1;
}

void MessageHandler::onGetDogStatusReq(const TcpMessage& msg)
{
	TcpMessage m;
	DogStatusRsp_t rsp;
	rsp.code = 1000;
	rsp.dogStatus = _session->dogService().getCurrentStatus();
	m.body = s_jsonMessageProcessor.encodeDogStatusRsp(rsp);
	m.msgType = _messageTypes[kGetDogStatusRsp];
	_session->write(m);
}
