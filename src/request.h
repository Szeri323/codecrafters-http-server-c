struct REQUEST_HEADER
{
    char user_agent[30];
};

struct REQUEST
{
    // Request line
    char *method;
    char *http_v;
    char host[30];
    char path[30];

    // Headers
    struct REQUEST_HEADER request_header;
    int content_length;
    char *accept_encoding;

    // Body
    char body[100];
};