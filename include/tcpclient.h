
#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include <boost/asio.hpp>
#include "tcp_message.hpp"
#include "messagebus.hpp"
#include "common.h"
#include "rockapi.h"

struct ClientConfig;
class TcpClientImpl;
class ROCK_NET_API TcpClient
{
public:
  TcpClient(boost::asio::io_context& io_context,
			const ClientConfig &cfg,
			const MessageType_t &messageTypes);

  ~TcpClient();

  // note: _syncBytes _msgSeq _bodyLen _fullMsgLen这三个字段会自动补全，可以不用填写
  void write(TcpMessage &msg);
  
  void close();
  
  MessageBus<> &messagebus();
  
  void start();

private:

	TcpClientImpl* _pImpl;
};

#endif
