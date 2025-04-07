#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include "request.h"
#include "response.h"
#include "header.h"

void parse_reqest_element(int i, int step, char break_char, char *dest, char *buf)
{
	int j = i + (step + 3);
	int k = 0;
	while (buf[j] != break_char)
	{
		dest[k] = buf[j];
		++j;
		++k;
	}
};

int process_cominication(int socket_fd)
{
	// Handling request
	struct REQUEST_HEADER request_header = {
		.user_agent = "",
	};

	struct REQUEST req = {
		.method = "",
		.http_v = "",
		.host = "",
		.path = "",
		.request_header = request_header,
	};

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

	printf("Parsing REQUEST:\n");
	printf("\n----REQUEST----\n");
	for (int i = 0; i < data_recv_size; ++i)
	{
		printf("%c", buf[i]);

		if (buf[i] == 'G' && buf[i + 2] == 'T')
		{
			parse_reqest_element(i, 2, ' ', req.path, buf);
		}

		if (buf[i] == 'U' && buf[i + 9] == 't')
		{
			parse_reqest_element(i, 9, '\r', request_header.user_agent, buf);
		}

		if (buf[i] == 'H' && buf[i + 3] == 't')
		{
			parse_reqest_element(i, 3, '\n', req.host, buf);
		}
	}
	printf("\n----END REQUEST----\n");

	// Looking for url path in request parameter

	// loking for user agent in request

	printf("\n");

	// Creating and sending response
	printf("CREATING RESPONSE:\n");
	struct RESPONSE res = {
		.http_v = "HTTP/1.1",
		.status_code = "404",
		.status_word = "Not Found",
	};

	char buf_res[50];

	struct HEADER header =
		{
			.content_type = "",
			.content_length = 0,
		};

	if (strlen(req.path) == 0)
	{
		res.status_code = "200";
		res.status_word = "OK";
	}
	else if (strstr(req.path, "echo") != NULL)
	{
		res.status_code = "200";
		res.status_word = "OK";
		int req_path_length = strlen(req.path);
		char value[30];
		int i = 5;
		int j = 0;
		while (i < req_path_length)
		{
			value[j] = req.path[i];
			++i;
			++j;
		}
		header.content_type = "text/plain";
		header.content_length = strlen(value);

		strcpy(res.body, value);
	}
	else if (strstr(req.path, "user-agent") != NULL)
	{
		res.status_code = "200";
		res.status_word = "OK";

		header.content_type = "text/plain";
		header.content_length = strlen(request_header.user_agent);

		strcpy(res.body, request_header.user_agent);
	}

	// Status line
	strcpy(buf_res, "HTTP/1.1 ");
	strcat(buf_res, res.status_code);
	strcat(buf_res, " ");
	strcat(buf_res, res.status_word);
	if (strlen(res.body) != 0)
	{
		strcat(buf_res, "\r\n");
		// Headers
		strcat(buf_res, "Content-Type: ");
		strcat(buf_res, header.content_type);
		strcat(buf_res, "\r\n");
		strcat(buf_res, "Content-Length: ");
		char number[30];
		sprintf(number, "%d", header.content_length);
		strcat(buf_res, number);
		strcat(buf_res, "\r\n\r\n");
		// Response body
		strcat(buf_res, res.body);
	}
	else
	{
		strcat(buf_res, "\r\n\r\n");
	}

	ssize_t data_sent_size;

	printf("\n----RESPONSE----\n%s\n----END RESPONSE----\n", buf_res);

	data_sent_size = send(socket_fd, buf_res, strlen(buf_res), 0);
	if (data_sent_size == -1)
	{
		printf("Socket creation failed: %s...\n", strerror(errno));
		return 1;
	}

	printf("\nsended\n");
	return 0;
}

struct CONNECTION_PARAMS
{
	int server_fd;
	struct sockaddr_in client_addr;
	int client_addr_len;
};

void *thread_function_1(void *arg)
{
	struct CONNECTION_PARAMS *cp = arg;
	int socket_fd;
	socket_fd = accept(cp->server_fd, (struct sockaddr *)&cp->client_addr, &cp->client_addr_len);
	printf("Client connected\n");

	process_cominication(socket_fd);
}
void *thread_function_2(void *arg)
{
	struct CONNECTION_PARAMS *cp = arg;
	int socket_fd;
	socket_fd = accept(cp->server_fd, (struct sockaddr *)&cp->client_addr, &cp->client_addr_len);
	printf("Client connected\n");

	process_cominication(socket_fd);
}
void *thread_function_3(void *arg)
{
	struct CONNECTION_PARAMS *cp = arg;
	int socket_fd;
	socket_fd = accept(cp->server_fd, (struct sockaddr *)&cp->client_addr, &cp->client_addr_len);
	printf("Client connected\n");

	process_cominication(socket_fd);
}

int main()
{
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

	while (1)
	{

		struct CONNECTION_PARAMS cp = {
			.server_fd = server_fd,
			.client_addr = client_addr,
			.client_addr_len = client_addr_len,
		};

		status1 = pthread_create(&thread1, NULL, thread_function_1, &cp);
		if (status1)
		{
			printf("Error Thread1");
			return 1;
		}
		status2 = pthread_create(&thread2, NULL, thread_function_1, &cp);
		if (status2)
		{
			printf("Error Thread2");
			return 1;
		}
		status3 = pthread_create(&thread3, NULL, thread_function_1, &cp);
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
