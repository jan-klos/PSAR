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
	void send_file(char* bin_path, char* interface, char* file_path, char* address_dest)
	{
		char buf[256];
		strcpy(buf, bin_path);
		strcat(buf, " ");
		strcat(buf, interface);
		strcat(buf, " spray_wait ");
		strcat(buf, file_path);
		strcat(buf, " ");
		strcat(buf, address_dest);
		strcat(buf, " &");
		system(buf);
	}

	void sync_files(char* bin_path, char* interface)
	{
		char buf[256];
		strcpy(buf, bin_path);	
		strcat(buf, interface);
		strcat(buf, " epidemic &");
		system(buf);		
	}
};	

extern "C" 
{
    Protocol* Protocol_new(){ return new Protocol(); }
    void Protocol_send_file(Protocol* p, char* bin_path, char* interface, char* file_path, char* address_dest)
    { 
    	p->send_file(bin_path, interface, file_path, address_dest); 
    }
    void Protocol_sync_files(Protocol* p, char* bin_path, char* interface)
    {
    	p->sync_files(bin_path, interface);
    }
}
