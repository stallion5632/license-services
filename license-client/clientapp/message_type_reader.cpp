#include "message_type_reader.h"
#include "message_types_res.h"
#include "common.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/algorithm/string.hpp>
#include <iostream>

using namespace boost;
// load message_type file and create message maps
 MessageType_t loadMessageTypes(const std::string &filepath)
{
	MessageType_t messageTypes;

	property_tree::ptree pt;
	property_tree::json_parser::read_json(filepath, pt);

	auto parseHex = [](const std::string &str) {
		std::vector<std::string> fields;
		boost::split(fields, str, boost::is_any_of(","));
		static int out = 0; // out defined static  - solve Run-Time Check Failure #2 - Stack around the variable 'out' was corrupted.
		if (kIntLen == fields.size())
		{
			for (auto i = 0; i < kIntLen; i++)
			{
				auto field = boost::trim_copy(fields[i]);
				int x;
				std::stringstream ss;
				ss << std::hex << field;
				ss >> x;
				memset(((char *)&out) + i, x, 1);
			}
		}

		return out;
	};

	try
	{
		for (auto &element : pt)
		{
			if (element.first == kSyncBytes)
			{
				auto typ = parseHex(element.second.data());
				messageTypes.insert(std::pair<std::string, int>(element.first, typ));
			}
			else if (element.first == kMessageTypes)
			{
				for (auto &msgtype : element.second)
				{
					auto typ = parseHex(msgtype.second.data());
					messageTypes.insert(std::pair<std::string, int>(msgtype.first, typ));
				}
			}
			else
			{
				// key - value
				if (element.second.empty())
				{
					std::cout << "key - value:" << element.first << " = " << element.second.data() << std::endl;
				}
				else // just key
				{
					std::cout << element.first << std::endl;
				}
			}
		}
	}
	catch (const std::exception &)
	{
		std::cerr << "loadMessageTypes failed! " << std::endl;
	}

	return messageTypes;
}