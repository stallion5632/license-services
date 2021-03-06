
// 该文件对应于守护进程中的共享内存检查定时器的机制
// 如启用共享内存sequence检查，应该在worker进程中集成如下类似代码
// worker进程不用感知守护进程，共享内存不存在也不会导致worker工作不正常

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <iostream>
#include <string>

int main(int argc, char *argv[])
{
	using namespace boost::interprocess;
	try
	{
		while (1)
		{
			//Open already created shared memory object.
			shared_memory_object shm(open_only, "abcd", read_write);

			//Map the whole shared memory in this process
			mapped_region region(shm, read_write);

			// clear the memory for worker process, the other half memory is for daemon process
			std::memset(region.get_address(), 0, region.get_size() / 2);

			static uint64_t s_sequence = 0;
			s_sequence += 1;
			std::copy((char*)&s_sequence, (char*)&s_sequence + sizeof(s_sequence), (char*)region.get_address());
		
			std::cout << "current s_sequence is: " << s_sequence <<  std::endl;

			_sleep(1000);
		}
		
	}
	catch (const std::exception&)
	{
		std::cerr << "exception: " << std::endl;
	}

	return 0;
}