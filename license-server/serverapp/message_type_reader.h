#pragma once

#include <string>
#include <map>
using MessageType_t = std::map<std::string, int>;

const MessageType_t loadMessageTypes(const std::string &filepath);
