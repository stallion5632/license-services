#include "licenseclient-impl.h"
#include "tcpclient.h"
#include <iostream>
#include "message_types_res.h"
#include "json_message_processor.h"
#include "loghelper.h"

using namespace boost;
using namespace std;
using boost::asio::ip::tcp;
using namespace RockLog;

static JsonMessageProcessor s_processor;

void LicenseClientImpl::start(ClientConfig& cfg, MessageType_t& messageTypes, std::function<void(int)> cb)
{
	TcpClient client(_io_context, cfg, messageTypes);
	_client = &client;
	_messageTypes = messageTypes;

	// add message handlers
	client.messagebus().attach<>(kMsgStart, &LicenseClientImpl::onStart, *this);
	client.messagebus().attach<>(kMsgHeartbeatReq, &LicenseClientImpl::onSendHeartbeatReq, *this);
	client.messagebus().attach<TcpMessage>(kMsgReadHeader, &LicenseClientImpl::onReadHeader, *this);
	client.messagebus().attach<TcpMessage>(kMsgReadBody, &LicenseClientImpl::onReadBody, *this);
	regeditDogStatusCallback(cb);

	client.start();

	_io_context.run();
}

void LicenseClientImpl::stop()
{
	_io_context.stop();
	_client = nullptr;
}

void LicenseClientImpl::sendHeartbeatReq()
{
	if (_client == nullptr)
		return;

	TcpMessage msg;
	HeartbeatReq_t req;
	req.ip = boost::asio::ip::host_name();
	msg.msgType = _messageTypes[kHeartReq];
	msg.body = std::move(s_processor.encodeHeartbeatReq(req));
	_client->write(msg);
}

void LicenseClientImpl::sendLoginReq()
{
	if (_client == nullptr)
		return;

	TcpMessage msg;
	LoginReq_t req;
	req.ip = boost::asio::ip::host_name();
	msg.msgType = _messageTypes[kLoginReq];
	msg.body = std::move(s_processor.encodeLoginReq(req));
	_client->write(msg);
}

void LicenseClientImpl::sendLogoutReq()
{
	if (_client == nullptr)
		return;

	TcpMessage msg;
	LogoutReq_t req;
	req.ip = boost::asio::ip::host_name();
	msg.msgType = _messageTypes[kLogoutReq];
	msg.body = std::move(s_processor.encodeLogoutReq(req));
	_client->write(msg);
}

void LicenseClientImpl::sendGetDogStatusReq()
{
	if (_client == nullptr)
		return;

	TcpMessage msg;
	DogStatusReq_t req;
	req.ip = boost::asio::ip::host_name();
	msg.msgType = _messageTypes[kGetDogStatusReq];
	msg.body = std::move(s_processor.encodeDogStatusReq(req));
	_client->write(msg);
}

//--------------------- private functions -----------------------

void LicenseClientImpl::onStart()
{
	sendHeartbeatReq();
}

void LicenseClientImpl::onSendHeartbeatReq()
{
	sendHeartbeatReq();
}

void LicenseClientImpl::onReadHeader(TcpMessage msg)
{
	if (_messageTypes[kHeartRsp] == msg.msgType)
	{
		LOG(kInfo) << "_messageTypes: " << kHeartRsp << std::endl;
	}
	else if (_messageTypes[kLoginRsp] == msg.msgType)
	{
		LOG(kInfo) << "_messageTypes: " << kLoginRsp << std::endl;
	}
	else if (_messageTypes[kLogoutRsp] == msg.msgType)
	{
		LOG(kInfo) << "_messageTypes: " << kLogoutRsp << std::endl;
	}
	else if (_messageTypes[kGetDogStatusRsp] == msg.msgType)
	{
		LOG(kInfo) << "_messageTypes: " << kGetDogStatusRsp << std::endl;
	}
	else if (_messageTypes[kDogStatusNtf] == msg.msgType)
	{
		LOG(kInfo) << "_messageTypes: " << kDogStatusNtf << std::endl;
	}
}

void LicenseClientImpl::onReadBody(TcpMessage msg)
{
	LOG(kInfo) << "msg.body: " << msg.body << "\n";
	if (_messageTypes[kHeartRsp] == msg.msgType)
	{
		LOG(kInfo) << "receive  HeartbeatRsp" << "\n";
	}
	else if (_messageTypes[kLoginRsp] == msg.msgType) // TODO
	{
		LoginRsp_t rsp;
		bool ret = s_processor.decodeLoginRsp(msg.body, rsp);
		if (ret)
		{
			_dogStatusCallback(rsp.dogStatus);
		}
	}
	else if (_messageTypes[kLogoutRsp] == msg.msgType)
	{
		LogoutRsp_t rsp;
		bool ret = s_processor.decodeLoginRsp(msg.body, rsp);
		if (ret)
		{
			_dogStatusCallback(rsp.dogStatus);
		}
	}
	else if (_messageTypes[kGetDogStatusRsp] == msg.msgType)
	{
		DogStatusRsp_t rsp;
		bool ret = s_processor.decodeDogStatusRsp(msg.body, rsp);
		if (ret)
		{
			_dogStatusCallback(rsp.dogStatus);
		}
	}
	else if (_messageTypes[kDogStatusNtf] == msg.msgType)
	{
		DogStatusNtf_t ntf;
		bool ret = s_processor.decodeDogStatusNtf(msg.body, ntf);
		if (ret)
		{
			_dogStatusCallback(ntf.dogStatus);
		}
	}
}

void LicenseClientImpl::regeditDogStatusCallback(std::function<void(int)> cb)
{
	_dogStatusCallback = cb;
}