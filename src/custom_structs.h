#include <netinet/in.h>

struct CONNECTION_PARAMS
{
	int server_fd;
	struct sockaddr_in client_addr;
	int client_addr_len;
};

struct ARGUMENTS
{
	int argc;
	char **argv;
};

struct DATA_TO_PASS
{
	struct CONNECTION_PARAMS cp;
	struct ARGUMENTS args;
};
