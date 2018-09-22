#pragma once
#include <chrono>
#include <stdint.h>

namespace Utility
{
	static uint64_t getTimeStamp()
	{
		std::chrono::milliseconds ms = std::chrono::duration_cast< std::chrono::milliseconds >(
			std::chrono::system_clock::now().time_since_epoch()
			);

		return ms.count();
	}
}