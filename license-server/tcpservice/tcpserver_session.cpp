#include "tcpserver_session.h"
#include <iostream>
#include "bytes_buffer.hpp"
#include "common.h"
#include "loghelper.h"
#include <thread>
#include <chrono>
#include "message_types_res.h"


using namespace RockLog;
using namespace boost;
using boost::asio::deadline_timer;
using boost::asio::ip::tcp;

void TcpServerSession::write(TcpMessage &msg)
{
	msg.syncBytes = _messageTypes[kSyncBytes];
	_currentSeq += 1;
	msg.msgSeq = _currentSeq;
	msg.bodyLen = msg.body.length();
	msg.fullMsgLen = kTotalHeadLen + msg.bodyLen + msg.binaryData.length();

	_io_context.post(std::bind(&TcpServerSession::startWrite, shared_from_this(), msg));
}

void TcpServerSession::close()
{
	_messagehandler.releaseLicense();
	_sessionManger.leave(shared_from_this());
	_io_context.post(std::bind(&TcpServerSession::doClose, shared_from_this()));
}

void TcpServerSession::start()
{
	_sessionManger.join(shared_from_this());
	_stopped = false;
	startRead();

	_messagehandler.start();

	_heartbeatTimer.expires_from_now(boost::posix_time::seconds(10));
	_heartbeatTimer.async_wait(std::bind(&TcpServerSession::checkHeartbeat, shared_from_this()));
}

//----- private functions -----

void TcpServerSession::startRead()
{
	// Start an asynchronous operation to read a message.
	_rbuf.clear();
	_rbuf.resize(kTotalHeadLen);
	boost::asio::async_read(_socket,
							boost::asio::buffer((char *)_rbuf.c_str(), kTotalHeadLen),
							std::bind(&TcpServerSession::handleReadHeader, shared_from_this(), std::placeholders::_1));
}

void TcpServerSession::handleReadHeader(const boost::system::error_code &ec)
{
	if (_stopped)
		return;

	if (!ec)
	{
		BytesBuffer buf;
		buf.append(_rbuf.data(), _rbuf.length());

		buf.retrieve((char *)&_readMsg.syncBytes, kIntLen);
		if (_messageTypes[kSyncBytes] != _readMsg.syncBytes)
		{
			LOG(kErr) << "error sync_bytes! syncBytes: " << _readMsg.syncBytes << std::endl;
			close();
			return;
		}

		buf.retrieve((char *)&_readMsg.fullMsgLen, kIntLen);
		if (kMaxMessageSize < _readMsg.fullMsgLen)
		{
			LOG(kErr) << "message too big! fullMsgLen: " << _readMsg.fullMsgLen << std::endl;
			close();
			return;
		}

		buf.retrieve((char *)&_readMsg.msgSeq, kIntLen);
		if (_currentSeq > _readMsg.msgSeq)
		{
			LOG(kErr) << "message Seq error! msgSeq: " << _readMsg.msgSeq << std::endl;
			close();
			return;
		}
		_currentSeq = _readMsg.msgSeq;

		buf.retrieve((char *)&_readMsg.msgType, kIntLen); // TODO check msgType

		buf.retrieve((char *)&_readMsg.bodyLen, kIntLen);
		if (kMaxBodySize < _readMsg.bodyLen)
		{
			LOG(kErr) << "bodyLen too big! bodyLen: " << _readMsg.bodyLen << std::endl;
			close();
			return;
		}

		// reset read buffer size
		_rbuf.resize(_readMsg.fullMsgLen);

		boost::asio::async_read(_socket,
								boost::asio::buffer((char *)_rbuf.c_str() + kTotalHeadLen, _readMsg.fullMsgLen - kTotalHeadLen),
								std::bind(&TcpServerSession::handleReadBody, shared_from_this(), std::placeholders::_1));

		// received heartbreat request
		if (_messageTypes[kHeartReq] == _readMsg.msgType)
		{
			_isRecvedHeartbeatReq = true;
		}
		else
		{
			bool find = false;
			for (auto &it : _messageTypes)
			{
				if (it.second == _readMsg.msgType)
				{
					find = true;
					break;
				}
			}
			if (!find)
			{
				LOG(kWarn) << "unknown message receved! - msgType: " << _readMsg.msgType << std::endl;
				return;
			}
		}
		_messagehandler.readHeader(_readMsg);
	}
	else
	{
		if (ec == boost::asio::error::eof)
		{
			LOG(kErr) << "disconneted with server! ec.message: " << ec.message() << "\n";

			_socket.close();
			_stopped = true;
		}
		else
		{
			LOG(kErr) << "Error on handleReadHeader: " << ec.message() << "\n";
			close();
		}
	}
}

void TcpServerSession::handleReadBody(const boost::system::error_code &ec)
{
	if (_stopped)
		return;

	if (!ec)
	{
		BytesBuffer buf;
		buf.append(_rbuf.c_str() + kTotalHeadLen, _readMsg.fullMsgLen - kTotalHeadLen);

		if (_readMsg.bodyLen > 0)
		{
			auto body = buf.retrieveAsString(_readMsg.bodyLen);
			_readMsg.body = body;
		}

		auto binaryLen = _readMsg.fullMsgLen - kTotalHeadLen - _readMsg.bodyLen;
		if (binaryLen > 0)
		{
			auto binaryData = buf.retrieveAsString(binaryLen);
			_readMsg.binaryData = binaryData;
		}

		_messagehandler.readBody(_readMsg);

		_rbuf.clear();
		_rbuf.resize(kTotalHeadLen);

		boost::asio::async_read(_socket,
								boost::asio::buffer((char *)_rbuf.c_str(), kTotalHeadLen),
								std::bind(&TcpServerSession::handleReadHeader, shared_from_this(), std::placeholders::_1));
	}
	else
	{
		LOG(kErr) << "Error on handleReadBody: " << ec.message() << "\n";
		close();
	}
}

void TcpServerSession::doWrite(TcpMessage &msg)
{
	BytesBuffer buf;

	buf.append((char *)&msg.syncBytes, kIntLen);
	buf.append((char *)&msg.fullMsgLen, kIntLen);
	buf.append((char *)&msg.msgSeq, kIntLen);
	buf.append((char *)&msg.msgType, kIntLen);
	buf.append((char *)&msg.bodyLen, kIntLen);
	buf.append(msg.reserved, 3 * kIntLen);		// 12 Bytes reserved
	buf.append(msg.body);
	buf.append(msg.binaryData);

	_wbuf.clear();
	_wbuf.resize(msg.fullMsgLen);
	std::copy(buf.begin(), buf.begin() + buf.readableBytes(), _wbuf.begin());

	// Start an asynchronous operation to send a message.
	boost::asio::async_write(_socket,
							 boost::asio::buffer((char *)_wbuf.c_str(), _wbuf.length()),
							 std::bind(&TcpServerSession::handleWrite, shared_from_this(), std::placeholders::_1));
}

void TcpServerSession::startWrite(const TcpMessage &msg)
{
	if (_stopped)
		return;

	bool write_in_progress = !_writeMsgs.empty();
	_writeMsgs.push_back(msg);
	if (!write_in_progress)
	{
		auto &front = _writeMsgs.front();
		doWrite(front);
	}
}

void TcpServerSession::handleWrite(const boost::system::error_code &ec)
{
	if (_stopped)
		return;

	if (!ec)
	{
		_writeMsgs.pop_front();
		if (!_writeMsgs.empty())
		{
			auto &front = _writeMsgs.front();
			doWrite(front);
		}
	}
	else
	{
		LOG(kErr) << "Error on handleWrite: " << ec.message() << "\n";
		close();
	}
}

void TcpServerSession::checkHeartbeat()
{
	if (_stopped)
		return;

	if (_heartbeatTimer.expires_at() <= deadline_timer::traits_type::now())
	{
		if (_isRecvedHeartbeatReq)
		{
			// send heartbeat message
			_isRecvedHeartbeatReq = false;
			_heartBeatLostTimes = 0; // re-init
		}
		else
		{
			// heartbeat timeout
			if (_heartBeatLostTimes >= 3) // lost three times
			{
				LOG(kErr) << "heart beat timeout three times, disconnect!" << std::endl;
				close();
				_heartbeatTimer.expires_at(boost::posix_time::pos_infin);
			}
			else
			{
				_heartBeatLostTimes += 1;
			}
		}
		_heartbeatTimer.expires_from_now(boost::posix_time::seconds(_cfg.heartbeatInterval));
	}

	// Put the actor back to sleep.
	_heartbeatTimer.async_wait(std::bind(&TcpServerSession::checkHeartbeat, shared_from_this()));
}

// This function terminates all the actors to shut down the connection. It
// may be called by the user of the client class, or by the class itself in
// response to graceful termination or an unrecoverable error.
void TcpServerSession::doClose()
{
	_stopped = true;
	boost::system::error_code ignored_ec;
	_socket.close(ignored_ec);
	_heartbeatTimer.cancel();
}


