#ifndef HTTP_H
#define HTTP_H

struct HTTPRequest{
        char *method;
        char *path;
        char *version;
};

#endif