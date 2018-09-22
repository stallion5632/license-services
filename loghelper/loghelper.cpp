
#include "loghelper.h"
#include <stdint.h>
#include <sstream>
#include <iostream>
#include <thread>
#include <mutex>

using namespace RockLog;

#ifdef Use_OutputDebugString	// Cooperate with DebugView.exe
	#include <windows.h>
#endif

#ifdef Use_QDebug		// For Qt Debuging
	#include <QDebug>	
#endif

#ifdef Use_SysLog
	#include "sinks_syslog.hpp"
#endif

#ifdef Use_BoostLog
	#include "logger.hpp"		// boost.log, ref:https://github.com/contaconta/boost_log_example/blob/master/logger.hpp
#endif


LogHelper::LogHelper()
{
	_level = (int32_t)kDebug;
}

LogHelper::LogHelper(int32_t level, const char* func, uint32_t line)
	:_level(level), _funcName(func), _lineNo(line)
{
}

LogHelper::~LogHelper()
{
	std::ostringstream oss;

#if defined (Use_OutputDebugString) ||  defined (Use_QDebug) ||  defined (Use_stdout)

	switch (_level)
	{
	case (int32_t)kDebug:
		oss << "[D] -";
		break;
	case (int32_t)kInfo:
		oss << "[I] -";
		break;
	case (int32_t)kWarn:
		oss << "[W] -";
		break;
	case (int32_t)kErr:
		oss << "[E] -";
		break;
	case (int32_t)kDisable:
		return;
	default:
		break;
	}

#endif 

	oss << "[" << _funcName << ":" << _lineNo << "] - ";
	oss << _ss.str();

#ifdef Use_OutputDebugString
	oss << std::endl;
	OutputDebugStringA(oss.str().c_str());
#endif

#ifdef Use_QDebug
	qDebug() << oss.str().c_str();
#endif

#ifdef Use_stdout
	std::cout << oss.str() << std::endl;
#endif

#ifdef Use_SysLog
	using namespace SysLogger;
	static std::once_flag syslogFlag;
	std::call_once(syslogFlag, [] { init_builtin_syslog(); });

	src::severity_logger< > lg;

	oss << SysLogger::s_SysLogTag << " ";
	switch (_level)
	{
	case (int32_t)kDebug:
		BOOST_LOG_SEV(lg, SysLogger::DEBUG) << oss.str();
		break;
	case (int32_t)kInfo:
		BOOST_LOG_SEV(lg, SysLogger::INFO) << oss.str();
		break;
	case (int32_t)kWarn:
		BOOST_LOG_SEV(lg, SysLogger::WARNING) << oss.str();
		break;
	case (int32_t)kErr:
		BOOST_LOG_SEV(lg, SysLogger::ERROR) << oss.str();
		break;
	default:
		return;
	}
#endif

#ifdef Use_BoostLog
	static std::once_flag flag;
	std::call_once(flag, [] { logger::initLogging(); });
    	
    	switch (_level)
	{
	case (int32_t)kDebug:
		BOOST_LOG_SEV(app_logger::get(), logger::DEBUG) << oss.str();
		break;
	case (int32_t)kInfo:
		BOOST_LOG_SEV(app_logger::get(), logger::INFO) << oss.str();
		break;
	case (int32_t)kWarn:
		BOOST_LOG_SEV(app_logger::get(), logger::WARNING) << oss.str();
		break;
	case (int32_t)kErr:
		BOOST_LOG_SEV(app_logger::get(), logger::ERROR) << oss.str();
		break;
	default:
		return;
	}
#endif

}
	




