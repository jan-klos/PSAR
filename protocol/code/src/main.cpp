#include <cstdlib>				/*EXIT_SUCCESS/FAILURE*/
#include <iostream>
#include <thread>

#include <net/if.h>
#include <unistd.h>

#include "Dtn.hpp"
#include "Utils.hpp"

void usage(const char *prog_name)
{
	printf("Usage:\n");
	printf("%s <interface>\n", prog_name);
}

//TODO Config file
int checkCfg()
{
	printf("Config file checked\n");
	return 1;
}

int main(int argc, char **argv)
{
	uint network_id;
	char hostname[BUFF_SIZE];

	if(argc < 2)
	{
		printf("Bad args !\n");
		usage(argv[0]);
		return EXIT_FAILURE;
	}

	if(gethostname(hostname, BUFF_SIZE) < 0)
	{
		perror("gethostname failed");
		return EXIT_FAILURE;
	}

	Log log(hostname);

	if(log.init())
		return EXIT_FAILURE;

	network_id = if_nametoindex(argv[1]);
	if(!network_id)
	{
		LOG_PERROR(log);
		return EXIT_FAILURE;
	}
	
	// int my_id = atoi(hostname + LEN_RPI_NAME);
	// Dtn me(network_id, my_id, log);

	if(strcmp(argv[2], "epidemic") == 0)
	{
		Dtn me(network_id, log);
		me.start();
		log_notice(log, "DTN initialized - Epidemic\n");
	}
	else if(strcmp(argv[2], "spray_wait") == 0)
	{
		Dtn me(network_id, log, argv[3], argv[4]);
		me.start();
		log_notice(log, "DTN initialized - Spray & Wait\n");
	}

	return EXIT_SUCCESS;
}