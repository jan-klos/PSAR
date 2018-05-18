#pragma once

#include <iostream>
#include <mutex>

#include <time.h>

#include "Utils.hpp"

#define LOG_PATH		"log/"
#define LOG_SUFF		".txt"

#define LOG_NOTICE		"  OK  "
#define LOG_INFO		" INFO "
#define LOG_DEBUG		" DBUG "
#define LOG_WARN		" WARN "
#define LOG_ALERT		" FAIL "

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define LOG(log, type, color, ...)\
	do {\
		struct timespec tv;\
		char log_text[MAX_LOG_MSG_SIZE], *offset;\
		int space_left;\
\
		offset = log_text;\
\
		clock_gettime(CLOCK_REALTIME, &tv);\
		unsigned long long ts =\
			(unsigned long long)(tv.tv_sec) * 1000 +\
			(unsigned long long)(tv.tv_nsec) / 1000000;\
\
		offset += snprintf(offset, MAX_LOG_MSG_SIZE, "[%llu][ %s ][" type "] ",\
							ts, log.getHostname());\
\
		space_left = MAX_LOG_MSG_SIZE-(offset-log_text);\
		snprintf(offset, space_left, __VA_ARGS__);\
\
		log.log_lock();\
		fprintf(log.getFile(), "%s", log_text);\
		fflush(log.getFile());\
		log.log_unlock();\
\
	printf(color "[" type "] %s" ANSI_COLOR_RESET, offset);\
	} while (0)

#define log_notice(log, ...)\
	do {\
		LOG(log, LOG_NOTICE, ANSI_COLOR_GREEN, __VA_ARGS__);\
	} while (0)

#define log_info(log, ...)\
	do {\
		LOG(log, LOG_INFO, "" ,__VA_ARGS__);\
	} while (0)

#define log_debug(log, ...)\
	do {\
		LOG(log, LOG_DEBUG, "" ,__VA_ARGS__);\
	} while (0)

#define log_warn(log, ...)\
	do {\
		LOG(log, LOG_WARN, ANSI_COLOR_YELLOW, __VA_ARGS__);\
	} while (0)

#define log_alert(log, ...)\
	do {\
		LOG(log, LOG_ALERT, ANSI_COLOR_RED, __VA_ARGS__);\
	} while (0)

class Log
{
	private:
		FILE* log_file;
		std::mutex log_mutex;
		char *hostname;

	public:
		Log(char * hostname);
		~Log();
		int init();
		FILE* getFile();
		char* getHostname();
		void log_lock();
		void log_unlock();
};