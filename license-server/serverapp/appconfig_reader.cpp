#include "appconfig_reader.h"
#include "common.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/algorithm/string.hpp>
#include <iostream>

using namespace boost;

static const char *kAppConfigRoot = "server";
static const char *kDogCheckInterval = "DogCheckInterval";
static const char *kHeartbeatInterval = "HeartbeatInterval";
static const char *kMaxSupportClientNum = "MaxSupportClientNum";
static const char *kServerPort = "ServerPort";

// load app configure
ServerConfig loadAppConfig(const std::string &filepath)
{
	property_tree::ptree pt;
	property_tree::ini_parser::read_ini(filepath, pt);
	property_tree::ptree client;
	client = pt.get_child(kAppConfigRoot);

	ServerConfig cfg;
	auto a = client.get_optional<int>(kHeartbeatInterval);
	if (a)
		cfg.heartbeatInterval = *a;

	auto b = client.get_optional<int>(kDogCheckInterval);
	if (b)
		cfg.dogCheckInterval = *b;

	auto c = client.get_optional<int>(kMaxSupportClientNum);
	if (c)
		cfg.maxSupportClientNum = *c;

	auto d = client.get_optional<int>(kServerPort);
	if (d)
		cfg.serverPort = *d;

	return cfg;
}
