#pragma once

#include "rockapi.h"
#include <functional>

class ROCK_DOG_API DogService
{
public:
	DogService();
	~DogService();

	int start();
	void stop();
	int getCurrentStatus();
	void updateSessionStatus(std::function< void(int) > cb);
	void setTimerInterval(int interval);

};

