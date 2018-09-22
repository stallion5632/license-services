#include "tcpclient_impl.h"
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

void TcpClientImpl::write(TcpMessage &msg)
{
	msg.syncBytes = _messageTypes[kSyncBytes];
	_currentSeq += 1;
	msg.msgSeq = _currentSeq;
	msg.bodyLen = msg.body.length();
	msg.fullMsgLen = kTotalHeadLen + msg.bodyLen + msg.binaryData.length();

	_io_context.post(std::bind(&TcpClientImpl::startWrite, this, msg));
}

void TcpClientImpl::close()
{
	_io_context.post(std::bind(&TcpClientImpl::doClose, this));
}

//----- private functions -----

void TcpClientImpl::start()
{
	LOG(kInfo) << "Trying " << _endpoints->endpoint() << "...\n";

	// Start the asynchronous connect operation.
	boost::asio::async_connect(_socket, _endpoints,
							   std::bind(&TcpClientImpl::handleConnect, this, std::placeholders::_1));
}

void TcpClientImpl::handleConnect(const boost::system::error_code &ec)
{
	if (ec || !_socket.is_open())
	{
		if (ec)
		{
			LOG(kErr) << "Connected to server " << _endpoints->endpoint() << " failed!\n";
			_socket.close();
		}
		else
			LOG(kErr) << "Connected to server " << _endpoints->endpoint() << " timeout!\n";

		_stopped = true;

		if (!_clientConfig.isReconnect)
			throw boost::system::system_error(ec ? ec : boost::asio::error::operation_aborted);
		else
			startReconnect();

		return;
	}
	// Otherwise we have successfully established a connection.
	else
	{
		_stopped = false;
		LOG(kInfo) << "Connected to " << _endpoints->endpoint() << "\n";

		// program start
		_messagebus.sendMessage(kMsgStart);

		// Start the input actor.
		startRead();

		_heartbeatTimer.expires_from_now(boost::posix_time::seconds(_clientConfig.heartbeatInterval));
		_heartbeatTimer.async_wait(std::bind(&TcpClientImpl::checkHeartbeat, this));
		// sendHeartbeatReq(); // start heartbeat
		_isRecvedHeartbeatRsp = false;
	}
}

void TcpClientImpl::doReconnect(const boost::system::error_code &ec)
{
	LOG(kInfo) << "reconnect to server " << _endpoints->endpoint() << "......"
			   << "\n";
	// Start the asynchronous connect operation.
	boost::asio::async_connect(_socket, _endpoints,
							   std::bind(&TcpClientImpl::handleConnect, this, std::placeholders::_1));
}

void TcpClientImpl::startRead()
{
	// Start an asynchronous operation to read a message.
	_rbuf.clear();
	_rbuf.resize(kTotalHeadLen);
	boost::asio::async_read(_socket,
							boost::asio::buffer((char *)_rbuf.c_str(), kTotalHeadLen),
							std::bind(&TcpClientImpl::handleReadHeader, this, std::placeholders::_1));
}

void TcpClientImpl::handleReadHeader(const boost::system::error_code &ec)
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
								std::bind(&TcpClientImpl::handleReadBody, this, std::placeholders::_1));

		// received heartbreat respone
		if (_messageTypes[kHeartRsp] == _readMsg.msgType)
		{
			_isRecvedHeartbeatRsp = true;
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
		_messagebus.sendMessage(kMsgReadHeader, _readMsg);
	}
	else
	{
		if (ec == boost::asio::error::eof)
		{
			LOG(kErr) << "disconneted with server! ec.message: " << ec.message() << "\n";

			_socket.close();
			_stopped = true;
			if (_clientConfig.isReconnect)
			{
				std::this_thread::sleep_for(std::chrono::seconds(_clientConfig.reconnectInterval)); // sleep, then reconnect
				startReconnect();
			}
		}
		else
		{
			LOG(kErr) << "Error on handleReadHeader: " << ec.message() << "\n";
			close();
		}
	}
}

void TcpClientImpl::handleReadBody(const boost::system::error_code &ec)
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

		_messagebus.sendMessage(kMsgReadBody, _readMsg);

		_rbuf.clear();
		_rbuf.resize(kTotalHeadLen);

		boost::asio::async_read(_socket,
								boost::asio::buffer((char *)_rbuf.c_str(), kTotalHeadLen),
								std::bind(&TcpClientImpl::handleReadHeader, this, std::placeholders::_1));
	}
	else
	{
		LOG(kErr) << "Error on handleReadBody: " << ec.message() << "\n";
		close();
	}
}

void TcpClientImpl::doWrite(TcpMessage &msg)
{
	BytesBuffer buf;

	buf.append((char *)&msg.syncBytes, kIntLen);
	buf.append((char *)&msg.fullMsgLen, kIntLen);
	buf.append((char *)&msg.msgSeq, kIntLen);
	buf.append((char *)&msg.msgType, kIntLen);
	buf.append((char *)&msg.bodyLen, kIntLen);
	buf.append(msg.reserved, 3 * kIntLen); // 12 Bytes reserved
	buf.append(msg.body);
	buf.append(msg.binaryData);

	_wbuf.clear();
	_wbuf.resize(msg.fullMsgLen);
	std::copy(buf.begin(), buf.begin() + buf.readableBytes(), _wbuf.begin());

	// Start an asynchronous operation to send a message.
	boost::asio::async_write(_socket,
							 boost::asio::buffer((char *)_wbuf.c_str(), _wbuf.length()),
							 std::bind(&TcpClientImpl::handleWrite, this, std::placeholders::_1));
}

void TcpClientImpl::startWrite(const TcpMessage &msg)
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

void TcpClientImpl::handleWrite(const boost::system::error_code &ec)
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
		LOG(kErr) << "Error on write: " << ec.message() << "\n";
		close();
	}
}

void TcpClientImpl::checkHeartbeat()
{
	if (_stopped)
		return;

	if (_heartbeatTimer.expires_at() <= deadline_timer::traits_type::now())
	{
		if (_isRecvedHeartbeatRsp)
		{
			// send heartbeat message
			_messagebus.sendMessage(kMsgHeartbeatReq);
			_isRecvedHeartbeatRsp = false;
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
		_heartbeatTimer.expires_from_now(boost::posix_time::seconds(_clientConfig.heartbeatInterval));
	}

	// Put the actor back to sleep.
	_heartbeatTimer.async_wait(std::bind(&TcpClientImpl::checkHeartbeat, this));
}

// This function terminates all the actors to shut down the connection. It
// may be called by the user of the client class, or by the class itself in
// response to graceful termination or an unrecoverable error.
void TcpClientImpl::doClose()
{
	_stopped = true;
	boost::system::error_code ignored_ec;
	_socket.close(ignored_ec);
	_heartbeatTimer.cancel();
	_reconnectTimer.cancel();
}

void TcpClientImpl::startReconnect()
{
	// schedule a timer to reconnect after 5 seconds
	_reconnectTimer.expires_from_now(boost::posix_time::seconds(_clientConfig.reconnectInterval));
	_reconnectTimer.async_wait(std::bind(&TcpClientImpl::doReconnect, this, std::placeholders::_1));
}
