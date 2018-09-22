#pragma once


#include <boost/config.hpp>

#include <string>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared_object.hpp>
#include <boost/log/core.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/syslog_backend.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/attributes/scoped_attribute.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/filesystem.hpp>

namespace SysLogger
{
namespace logging = boost::log;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace keywords = boost::log::keywords;


// Complete sink type
typedef sinks::synchronous_sink< sinks::syslog_backend > sink_t;


#if !defined(BOOST_LOG_NO_ASIO)


static const char *kSysLogRoot = "SysLog";
static const char *kSysLogAddr = "SysLogAddr";
static const char *kSysLogPort = "SysLogPort";
static const char *kSysLogTag = "SysLogTag";


static std::string s_SysLogTag = "MySysLogger";

enum severity_level
{
	TRACE,
	DEBUG,
	INFO,
	WARNING,
	ERROR,
	FATAL
};

struct SysLogConfig
{
	std::string syslogAddr;
	int sysLogPort;
	std::string sysLogTag;
};

SysLogConfig loadSysLogConfig(const std::string &filepath)
{
	using namespace boost;
	property_tree::ptree pt;
	property_tree::ini_parser::read_ini(filepath, pt);
	property_tree::ptree client;
	client = pt.get_child(kSysLogRoot);

	SysLogConfig cfg;
	auto a = client.get_optional<std::string>(kSysLogAddr);
	if (a)
		cfg.syslogAddr = *a;

	auto b = client.get_optional<int>(kSysLogPort);
	if (b)
		cfg.sysLogPort = *b;

	auto c = client.get_optional<std::string>(kSysLogTag);
	if (c)
	{
		cfg.sysLogTag = *c;
		s_SysLogTag = *c;
	}

	return cfg;
}
static void  init_builtin_syslog()
{
	static bool inited = false;
	if (!inited)
		return;

	const std::string logFilePath = boost::filesystem::current_path().string() + std::string("/logger.cfg");
	SysLogConfig cfg;
	if (boost::filesystem::exists(logFilePath))
		cfg = std::move(loadSysLogConfig(logFilePath));
	else
	{
		inited = true;
		return;
	}
		

	boost::shared_ptr< logging::core > core = logging::core::get();

	// Create a new backend
	boost::shared_ptr< sinks::syslog_backend > backend(new sinks::syslog_backend(
		keywords::facility = sinks::syslog::local1,             /*< the logging facility >*/
		keywords::use_impl = sinks::syslog::udp_socket_based    /*< the built-in socket-based implementation should be used >*/
	));

	// Setup the target address and port to send syslog messages to
	backend->set_target_address(cfg.syslogAddr, cfg.sysLogPort);

	// Create and fill in another level translator for "MyLevel" attribute of type string
	sinks::syslog::custom_severity_mapping< std::string > mapping("MyLevel");
	mapping["debug"] = sinks::syslog::debug;
	mapping["normal"] = sinks::syslog::info;
	mapping["warning"] = sinks::syslog::warning;
	mapping["failure"] = sinks::syslog::critical;
	backend->set_severity_mapper(mapping);

	// Wrap it into the frontend and register in the core.
	core->add_sink(boost::make_shared< sink_t >(backend));

	inited = true;
}
}
#endif