#pragma once

#include <iostream>

class Communication
{
	public:
		virtual ~Communication() {};
		//Waiting for new peer detection
		virtual void start_monitoring(int *end) = 0;
};