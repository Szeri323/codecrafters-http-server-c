struct RESPONSE
{
    char *http_v;
    char *status_code;
    char *status_word;
    unsigned char body[100];
    int size_of_response;
};