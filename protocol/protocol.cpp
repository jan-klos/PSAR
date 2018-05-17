#include <stdio.h>      
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h> 
#include <string.h> 
#include <arpa/inet.h>
#include <iostream>

class Protocol
{
	public:
	void send_file(char* file_path, char* address_dest)
	{
		char buf[256];
		strcpy(buf, "/home/komp/PSAR/protocol/code/bin/main spray_wait wlan0 ");
		strcat(buf, file_path);
		strcat(buf, " ");
		strcat(buf, address_dest);
		strcat(buf, " &");
		system(buf);
	}

	void sync_files()
	{
		system("/home/komp/PSAR/protocol/code/bin/main epidemic wlan0");
	}
};	

extern "C" 
{
    Protocol* Protocol_new(){ return new Protocol(); }
    void Protocol_send_file(Protocol* p, char* file_path, char* address_dest)
    { 
    	p->send_file(file_path, address_dest); 
    }
    void Protocol_sync_files(Protocol* p)
    {
    	p->sync_files();
    }
}
