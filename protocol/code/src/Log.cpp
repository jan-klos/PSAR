#include <chrono>
#include <iomanip>
#include <sstream>

#include <stdio.h>

#include "Log.hpp"

using namespace std;

Log::Log(char * host)
{
	hostname = host;
}

Log::~Log()
{
	fclose(log_file);
}

int Log::init()
{
	ostringstream oss;
	std::time_t tt = chrono::system_clock::to_time_t (chrono::system_clock::now());
	struct std::tm * ptm = std::localtime(&tt);

	oss << put_time(ptm, "%d-%m_%H:%M:%S");
	string filename = LOG_PATH + oss.str() + "_" + hostname + LOG_SUFF;
	filename = "/home/pi/protocol_log";
	log_file = fopen(filename.c_str(), "w+");
	if(log_file == NULL)
	{
		perror("Failed to create log file");
		return -1;
	}
	return 0;
}

FILE* Log::getFile()
{
	return log_file;
}

char* Log::getHostname()
{
	return hostname;
}

void Log::log_lock()
{
	log_mutex.lock();
}

void Log::log_unlock()
{
	log_mutex.unlock();
}
