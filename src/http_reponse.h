#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

#include "http.h"

class Http_response  : public Http{
public:
    Http_response() {};
    virtual ~Http_response() {};
    virtual void handle() {};
    virtual void pre() {};    //  处理http请求或者响应的预处理操作

    int fd;

private:


};


#endif