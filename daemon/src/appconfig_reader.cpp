#include "appconfig_reader.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/algorithm/string.hpp>
#include <iostream>

using namespace boost;

static const char *kAppConfigRoot = "daemon";
static const char *kProcessName = "ProcessName";
static const char *kCheckProcessInterval = "CheckProcessInterval";
static const char *kSharedMemoryCheck = "SharedMemoryCheck";
static const char *kTotalSharedMemorySize = "TotalSharedMemorySize";
static const char *kSharedMemoryName = "SharedMemoryName";

// load app configure
Config_t loadAppConfig(const std::string &filepath)
{
	property_tree::ptree pt;
	property_tree::ini_parser::read_ini(filepath, pt);
	property_tree::ptree daemon;
	daemon = pt.get_child(kAppConfigRoot);

	Config_t cfg;
	auto a = daemon.get_optional<std::string>(kProcessName);
	if (a)
		cfg.processName = *a;

	auto b = daemon.get_optional<int>(kCheckProcessInterval);
	if (b)
		cfg.checkProcessInterval = *b;

	auto c = daemon.get_optional<bool>(kSharedMemoryCheck);
	if (c)
		cfg.sharedMemoryCheck = *c;

	auto d = daemon.get_optional<int>(kTotalSharedMemorySize);
	if (d)
		cfg.totalSharedMemorySize = *d;

	auto e = daemon.get_optional<std::string>(kSharedMemoryName);
	if (e)
		cfg.sharedMemoryName = *e;

	return cfg;
}
