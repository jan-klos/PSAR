#include <stdio.h>      
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h> 
#include <string.h> 
#include <arpa/inet.h>
#include <iostream>

class Foo{
	public:
	void execute()
	{
		int status = system("/home/komp/PSAR/protocol/code/bin/main /home/komp/log_test");
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
    Foo* Foo_new(){ return new Foo(); }
    void Foo_execute(Foo* foo){ foo->execute(); }
}
