#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

int main()
{
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

	int socket_fd;
	socket_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
	printf("Client connected\n");


	// Handling request
	char buf[1024];
	ssize_t data_recv_size;

	data_recv_size = recv(socket_fd, buf, sizeof(buf), 0);
	if (data_recv_size == -1)
	{
		printf("Socket creation failed: %s...\n", strerror(errno));
		return 1;
	}

	printf("data recv size: %zd\n", data_recv_size);

	printf("Response:\n");

	for (int i = 0; i < data_recv_size; ++i)
	{
		printf("%c", buf[i]);
	}

	printf("\n");

	// Creating and sending response
	char *buf_res = "HTTP/1.1 200 OK\r\n\r\n";
	ssize_t data_sent_size;

	// strlen + 1 this one is \0 sign at the end of all strings
	printf("%ld", strlen(buf_res));

	data_sent_size = send(socket_fd, buf_res, strlen(buf_res) + 1, 0);
	if (data_sent_size == -1)
	{
		printf("Socket creation failed: %s...\n", strerror(errno));
		return 1;
	}

	printf("%zd\n", data_sent_size);
	printf("sended\n");

	close(server_fd);
	printf("Connection closed\n");

	return 0;
}
