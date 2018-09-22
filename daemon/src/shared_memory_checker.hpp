#pragma once

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>

class SharedMemoryChecker
{
public:
	SharedMemoryChecker(const std::string& name, uint32_t size)
	{
		using namespace boost::interprocess;
		shared_memory_object::remove(name.c_str());
		//Create a shared memory object.
		_shm = std::move(shared_memory_object(create_only, name.c_str(), read_write));
		//Set size
		_shm.truncate(size);
	}

	// 0 -success
	// -1 - faild, but reach 3 times
	// -2 - process abnormal, need killed
	int check()
	{
		using namespace boost::interprocess;

		//Map the whole shared memory in this process
		mapped_region region(_shm, read_write);
		uint64_t sequence = 0;
		std::copy((char*)region.get_address(), (char*)region.get_address() + sizeof(sequence), (char*)&sequence);
		if (sequence <= _sequence && sequence != 0 )
		{
			_failedNum++;
			if (_failedNum == 3)
				return -2; // ops, process abnormal
			else
				return -1;
		}
		_sequence = sequence;
		return 0;
	}

private:
	boost::interprocess::shared_memory_object _shm;
	uint64_t _sequence = 0;
	int _failedNum = 0;
};
