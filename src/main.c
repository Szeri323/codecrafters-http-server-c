#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include "networking.h"

void *thread_function_1(void *arg)
{
	struct DATA_TO_PASS *dp = (struct DATA_TO_PASS *)arg;
	struct CONNECTION_PARAMS *cp = (struct CONNECTION_PARAMS *)&dp->cp;
	struct ARGUMENTS *args = &dp->args;
	int socket_fd;
	socket_fd = accept(cp->server_fd, (struct sockaddr *)&cp->client_addr, &cp->client_addr_len);
	printf("Client connected\n");

	process_commuinication(socket_fd, args);

	pthread_exit(NULL);
}
void *thread_function_2(void *arg)
{
	struct DATA_TO_PASS *dp = (struct DATA_TO_PASS *)arg;
	struct CONNECTION_PARAMS *cp = (struct CONNECTION_PARAMS *)&dp->cp;
	struct ARGUMENTS *args = &dp->args;
	int socket_fd;
	socket_fd = accept(cp->server_fd, (struct sockaddr *)&cp->client_addr, &cp->client_addr_len);
	printf("Client connected\n");

	process_commuinication(socket_fd, args);

	pthread_exit(NULL);
}
void *thread_function_3(void *arg)
{
	struct DATA_TO_PASS *dp = (struct DATA_TO_PASS *)arg;
	struct CONNECTION_PARAMS *cp = (struct CONNECTION_PARAMS *)&dp->cp;
	struct ARGUMENTS *args = &dp->args;
	int socket_fd;
	socket_fd = accept(cp->server_fd, (struct sockaddr *)&cp->client_addr, &cp->client_addr_len);
	printf("Client connected\n");

	process_commuinication(socket_fd, args);

	pthread_exit(NULL);
}

int main(int argc, char **argv)
{
	struct ARGUMENTS args = {
		.argc = argc,
		.argv = argv,
	};

	pthread_t thread1, thread2, thread3;
	int status1, status2, status3;

	// Disable output buffering
	setbuf(stdout, NULL);
	setbuf(stderr, NULL);

	// You can use print statements as follows for debugging, they'll be visible when running tests.
	printf("Logs from your program will appear here!\n");

	// Uncomment this block to pass the first stage
	int server_fd, client_addr_len;
	struct sockaddr_in client_addr;

	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd == -1)
	{
		printf("Socket creation failed: %s...\n", strerror(errno));
		return 1;
	}

	// Since the tester restarts your program quite often, setting SO_REUSEADDR
	// ensures that we don't run into 'Address already in use' errors
	int reuse = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
	{
		printf("SO_REUSEADDR failed: %s \n", strerror(errno));
		return 1;
	}

	struct sockaddr_in serv_addr = {
		.sin_family = AF_INET,
		.sin_port = htons(4221),
		.sin_addr = {htonl(INADDR_ANY)},
	};

	if (bind(server_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) != 0)
	{
		printf("Bind failed: %s \n", strerror(errno));
		return 1;
	}

	int connection_backlog = 5;
	if (listen(server_fd, connection_backlog) != 0)
	{
		printf("Listen failed: %s \n", strerror(errno));
		return 1;
	}

	printf("Waiting for a client to connect...\n");
	client_addr_len = sizeof(client_addr);

	struct CONNECTION_PARAMS cp = {
		.server_fd = server_fd,
		.client_addr = client_addr,
		.client_addr_len = client_addr_len,
	};

	struct DATA_TO_PASS dp = {
		.cp = cp,
		.args = args,
	};

	while (1)
	{
		status1 = pthread_create(&thread1, NULL, thread_function_1, &dp);
		if (status1)
		{
			printf("Error Thread1");
			return 1;
		}
		status2 = pthread_create(&thread2, NULL, thread_function_1, &dp);
		if (status2)
		{
			printf("Error Thread2");
			return 1;
		}
		status3 = pthread_create(&thread3, NULL, thread_function_1, &dp);
		if (status3)
		{
			printf("Error Thread3");
			return 1;
		}

		pthread_join(thread1, NULL);
		pthread_join(thread2, NULL);
		pthread_join(thread3, NULL);
	}

	// int socket_fd;
	// socket_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
	// printf("Client connected\n");

	// process_cominication(socket_fd);

	close(server_fd);
	printf("Connection closed\n");

	return 0;
}

// After 3 requests the 4th one makes error