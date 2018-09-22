
/*

消息    	               长度(BYTES)                	结构               	描述
Header	                   4                    	SyncBytes        	消息标识头，也可以标识不同设备之间的交互
Header	                   4                    	FullMessageLength	数据总长度，包括SyncBytes和FullMessageLength
Header	                   4                    	MessageSeq       	消息序列 seq ，递增
Header	                   4                    	MeaageType       	消息类型（消息号)，如登录消息或发送消息消息
Header	                   4                    	StringBody       	消息字符流长度，可能是json流、xml流以及普通字符流
Header	                   12                   	Reserved         	保留字段，总共12字节；各设备之间灵活使用，checksum也可放这
Body  	            StringBodyLength            	StringBody       	消息体（ json或xml或普通字符串数据），该字段长度可能为0
Body  	FullMessageLength - 32 - StringBodyLength	BinaryData       	二进制数据，图片数据轨迹等；该字段长度可能为0

*/

#ifndef TCP_MESSAGE_HPP
#define TCP_MESSAGE_HPP
#include <string>
#include <vector>

struct TcpMessage
{
	int syncBytes = 0;
	int fullMsgLen = 0;
	int msgSeq = 0;
	int msgType = 0;
	int bodyLen = 0;
    char reserved[12] = {0};	// reserved 12-bytes
	std::string body;
	std::string binaryData;	// maybe empty
};

#endif // CHAT_MESSAGE_HPP
