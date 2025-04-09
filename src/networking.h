#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include "request.h"
#include "response.h"
#include "header.h"
#include "custom_structs.h"

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

void parse_request_path_value(int i, int req_path_length, char *value, struct REQUEST req)
{
    int j = 0;
    while (i < req_path_length)
    {
        value[j] = req.path[i];
        ++i;
        ++j;
    }
}

int process_commuinication(int socket_fd, void *args)
{
    struct ARGUMENTS *arguments = (struct ARGUMENTS *)args;
    int argc = arguments->argc;
    char **argv = arguments->argv;

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
        .body = "",
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
            req.method = "GET";
            parse_reqest_element(i, 2, ' ', req.path, buf);
        }
        if (buf[i] == 'P' && buf[i + 3] == 'T')
        {
            req.method = "POST";
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
        if (buf[i] == 'C' && buf[i + 6] == 't' && buf[i + 8] == 'L' && buf[i + 13] == 'h')
        {
            int j = i + 16;
            int k = 0;
            char number[10];
            for (int i = 0; i < 10; ++i)
            {
                number[i] = 0;
            }

            while (buf[j] != '\r')
            {
                number[k] = buf[j];
                ++j;
                ++k;
            }
            number[k] = '\0';

            req.content_length = atoi(number);
        }
        if (buf[i] == '\r' && buf[i + 1] == '\n' && buf[i + 2] == '\r' && buf[i + 3] == '\n')
        {
            int k = 0;
            printf("\n%d\n", req.content_length);
            printf("\ni: %d\n", i+4);
            for (int j = i + 4; j < data_recv_size; ++j, ++k)
            {
                printf("%c", buf[j]);
                req.body[k] = buf[j];
            }

            // printf("REQUEST BODY: %s\n", req.body);
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

    struct HEADER header = {
        .content_type = "",
        .content_length = 0,
    };

    if (strstr(req.method, "GET"))
    {
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
            for (int i = 0; i < 30; ++i)
            {
                value[i] = 0;
            }
            parse_request_path_value(5, req_path_length, value, req);
            printf("value of echo: %s\n", value);
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
        else if ((strstr(req.path, "files") != NULL) && argc == 3)
        {
            FILE *file;
            int req_path_length = strlen(req.path);
            char *tmp = argv[2];
            char value[50];
            for (int i = 0; i < 50; ++i)
            {
                value[i] = 0;
            }
            parse_request_path_value(6, req_path_length, value, req);

            file = fopen(strcat(tmp, value), "r");

            if (file != NULL)
            {
                int i = 0;
                char c;
                char file_content[100];
                while ((c = fgetc(file)) != EOF)
                {
                    file_content[i] = c;
                    ++i;
                }
                fclose(file);
                res.status_code = "200";
                res.status_word = "OK";
                header.content_type = "application/octet-stream";
                header.content_length = strlen(file_content);
                strcpy(res.body, file_content);
            }
        }
    }
    else if (strstr(req.method, "POST"))
    {
        if ((strstr(req.path, "files") != NULL) && argc == 3)
        {
            FILE *file;
            int req_path_length = strlen(req.path);
            char *tmp = argv[2];
            char value[50];
            for (int i = 0; i < 50; ++i)
            {
                value[i] = 0;
            }
            parse_request_path_value(7, req_path_length, value, req);

            file = fopen(strcat(tmp, value), "w");
            fputs(req.body, file);

            fclose(file);
            res.status_code = "201";
            res.status_word = "Created";
        }
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
    printf("buf RESSSSSSSS: %s\n", buf_res);
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