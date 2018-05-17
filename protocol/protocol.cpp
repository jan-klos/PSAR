#include <stdio.h>      
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h> 
#include <string.h> 
#include <arpa/inet.h>
#include <iostream>

class Protocol{
	public:
	void execute(char* file_path, char* address_dest)
	{
		char buf[256];
		strcpy(buf, "/home/komp/PSAR/protocol/code/bin/main wlan0 ");
		strcat(buf, file_path);
		strcat(buf, " ");
		strcat(buf, address_dest);
		strcat(buf, " &");
		//printf("%s", buf);
		int status = system(buf);
	}
	/*int bar()
	{
	    struct ifaddrs * ifAddrStruct=NULL;
	    struct ifaddrs * ifa=NULL;
	    void * tmpAddrPtr=NULL;
	
	    getifaddrs(&ifAddrStruct);
	
	    for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) 
	    {
	        if (!ifa->ifa_addr) 
	        {
	            continue;
	        }
	        if (ifa->ifa_addr->sa_family == AF_INET) 
	        { 
	            tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
	            char addressBuffer[INET_ADDRSTRLEN];
	            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
	            printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer); 
	        } 
	    }
	    if (ifAddrStruct!=NULL) 
	        freeifaddrs(ifAddrStruct);
	    return 0;
	}*/
};	

extern "C" {
    Protocol* Protocol_new(){ return new Protocol(); }
    void Protocol_execute(Protocol* p, char* file_path, char* address_dest)
    { 
    	p->execute(file_path, address_dest); 
    }
}
