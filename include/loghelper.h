#ifndef _LOG_HELPER_HPP
#define _LOG_HELPER_HPP

#include <stdint.h>
#include <sstream>
#include <iostream>
#include "rockapi.h"

namespace RockLog {

enum LogLevel
{
	kDisable	= -1,
	kDebug		= 0,
	kInfo		= 1,
	kWarn		= 2,
	kErr		= 3,
};

class ROCK_LOG_API LogHelper
{
public:
	LogHelper();

	LogHelper(int32_t level, const char* func, uint32_t line);

	LogHelper&  operator<<(std::ostream& (*log)(std::ostream&))
	{
		_ss << log;
		return *this;
	}

	template <typename T>
	LogHelper& operator<<(const T& log) 
	{
		if (_ss.str().length() > 0)
		{
			_ss << " ";
		}
		_ss << log;
		return *this;
	}

	~LogHelper();

private:
	int32_t				_level;
	std::stringstream	_ss; 
	std::string			_funcName;
	uint32_t			_lineNo;
};

#define LOG(X)		LogHelper(X, __FUNCTION__, __LINE__)
}

#endif
