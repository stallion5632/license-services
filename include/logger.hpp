/* -8- ***********************************************************************
 *
 *  logger.hpp
 *
 *                                          Created by ogata on 11/26/2013
 *                 Copyright (c) 2013 ABEJA Inc. All rights reserved.
 * ******************************************************************** -8- */

#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <iostream>
#include <boost/log/common.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/attributes/timer.hpp>
#include <boost/log/attributes/named_scope.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/sources/global_logger_storage.hpp>


namespace logger
{
namespace logging = boost::log;
namespace sinks = boost::log::sinks;
namespace attrs = boost::log::attributes;
namespace src = boost::log::sources;
namespace expr = boost::log::expressions;
namespace keywords = boost::log::keywords;

/**
 * @brief The severity_level enum
 *  Define application severity levels.
 */
enum severity_level
{
  TRACE,
  DEBUG,
  INFO,
  WARNING,
  ERROR,
  FATAL
};

// The formatting logic for the severity level
template< typename CharT, typename TraitsT >
inline std::basic_ostream< CharT, TraitsT >& operator<< (
    std::basic_ostream< CharT, TraitsT >& strm, severity_level lvl)
{
  static const char* const str[] =
  {
    "TRACE",
    "DEBUG",
    "INFO",
    "WARNING",
    "ERROR",
    "FATAL"
  };
  if (static_cast< std::size_t >(lvl) < (sizeof(str) / sizeof(*str)))
    strm << str[lvl];
  else
    strm << static_cast< int >(lvl);
  return strm;
}


static inline void setLoggingLevel(logger::severity_level level)
{
  logging::core::get()->set_filter(
    expr::attr< severity_level >("Severity") >= level
  );
}


static inline void initLogging()
{
	/*
	auto console_sink =logging::add_console_log(
		std::clog,
		keywords::format = expr::stream
		<< expr::format_date_time< boost::posix_time::ptime >("TimeStamp", "%Y-%m-%d, %H:%M:%S.%f")
		<< " [" << expr::format_date_time< attrs::timer::value_type >("Uptime", "%O:%M:%S")
		<< "] [" << expr::format_named_scope("Scope", keywords::format = "%n (%f:%l)")
		<< "] <" << expr::attr< severity_level >("Severity")
		<< "> " << expr::message);
	//auto console_sink = logging::add_console_log(std::clog, keywords::format = "%TimeStamp%: %Message%");
	*/
	auto console_sink =logging::add_console_log(
        std::clog,
  		keywords::format = expr::stream
		<< expr::format_date_time< boost::posix_time::ptime >("TimeStamp", "%Y-%m-%d %H:%M:%S.%f")
		<< " <" << expr::attr< severity_level >("Severity")
		<< "> " << expr::message);
	
	auto file_sink = logging::add_file_log
	(
		keywords::filter = expr::attr< severity_level >("Severity") >= DEBUG, //设置打印级别
		keywords::format = expr::stream
		<< expr::format_date_time< boost::posix_time::ptime >("TimeStamp", "%Y-%m-%d %H:%M:%S.%f")
		<< " <" << expr::attr< severity_level >("Severity")
		<< "> " << expr::message,
		keywords::file_name = "logs/%Y-%m-%d_%N.log",		//文件名，注意是全路径
		keywords::rotation_size = 5 * 1024 * 1024,			//单个文件限制大小
		keywords::open_mode = std::ios_base::app			//追加
		//keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0)    //每天重建
	);

	file_sink->locked_backend()->set_file_collector(sinks::file::make_collector(
		keywords::target = "logs",							//文件夹名
		keywords::max_size = 50 * 1024 * 1024,				//文件夹所占最大空间
		keywords::min_free_space = 100 * 1024 * 1024		//磁盘最小预留空间
	));

	file_sink->locked_backend()->scan_for_files();
	file_sink->locked_backend()->auto_flush(true);

	// Also let's add some commonly used attributes, like timestamp and record counter.
	logging::add_common_attributes();
	logging::core::get()->add_thread_attribute("Scope", attrs::named_scope());
	logging::core::get()->add_sink(console_sink);
	logging::core::get()->add_sink(file_sink);
	
}

} // end of namespace




BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(app_logger, boost::log::sources::severity_logger<logger::severity_level>)

#endif // LOGGER_HPP
