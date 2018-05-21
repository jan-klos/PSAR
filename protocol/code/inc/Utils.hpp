#pragma once

#include <array>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <openssl/md5.h>
#include <string.h>

#include "SprayWait.hpp"

/***** Leader Election *****/
#define INT_INVAL				-10
#define CHUNK_SIZE 				32

#define BUFF_SIZE 				512
#define MAX_LOG_MSG_SIZE		256

#define PORT 					7890

#define MESH_RESCAN_DELAY_SEC	1
#define FIRST_RESP_DELAY_SEC	2

#define LIMIT_INACTIVE_TIME_MS	5000
#define MIN_SIGNAL_VALUE_DBM	-30

#define BROADCAST_LIMIT			50
#define BROADCAST_DELAY_MSEC	100

#define LEN_RPI_NAME			6

#define LOOP_CHECK_DELAY_SEC	2

#define CHECK(x) do { \
  int retval = (x); \
  if (retval != 0) { \
    log_alert(stderr, "Runtime error: %s returned %d at %s:%d\n", #x, retval, __FILE__, __LINE__); \
    return; \
  } \
} while (0)

#define LOG_PERROR(log) do { \
    log_alert(log, "Runtime error at %s:%d -> %s\n",\
					__FILE__, __LINE__, strerror(errno)); \
} while (0)

typedef unsigned int uint;

void usage(const char *prog_name);

std::string exec(const char* cmd);

std::vector<std::string> split(const std::string &s, char delim);

void mac_addr_n2a(std::string &mac_addr, unsigned char *arg);

std::vector<std::string> split_string(std::string str);

void convert_file_to_bytes(const char* FILES_DIR, const char* filename, char* content);

void create_file(const char* FILES_DIR, std::string str); 

void create_file_sw(const char* FILES_DIR, std::string str, struct sw_struct* s); 

void get_file_hash(const char* FILES_DIR, const char* filename, char* hash);

void get_filepath(const char* FILES_DIR, const char* filename, char* filepath);