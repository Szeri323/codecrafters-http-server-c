struct REQUEST_HEADER {
    char user_agent[30];
};


struct REQUEST
{
    char *method;
    char *http_v;
    char host[30];
    char path[30];
    struct REQUEST_HEADER request_header;
    int content_length;
    char body[100];
};