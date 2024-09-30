#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include "http.h"

#define DECODE_STRING "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"
#define WEBSOCKET_KEY "Sec-WebSocket-Key"

class Http_request : public Http{
public:
    Http_request() {};
    virtual ~Http_request() {};
    void init();
    void init(int, struct sockaddr_in);
    bool read();    //  将http请求从内核放入应用程序
    bool write();   //  将http响应发送出去
    void process(MYSQL *);  //  处理http请求

    int read_idx; // 代表http请求的长度
    int write_idx;  //  代表http响应的长度
    int check_idx;
    char read_buf[READ_BUF_SIZE];
    char write_buf[WRITE_BUF_SIZE];

private:
    // bool handle_request();
    Http::LINE_STATUS parse_line();
    void process_read(MYSQL *);
    void process_write();
    char *get_line() { return read_buf + start_line; };
    Http::HTTP_CODE parse_request_line(char *);

    std::map<std::string, std::string> parse_query_params(const std::string&);

    std::string request_post;   //  http请求报文内容
    std::string response_post;  //  http响应报文内容
    std::string method;
    std::string url;
    std::string version;
    std::string path;   //  http请求api
    char *file_address;
    sockaddr_in address;
    std::string response_data;  //  protobuf序列化之后的数据

    int bytes_to_send;
    int bytes_have_send;
    char real_file[FILENAME_LEN];
    struct stat file_stat;
    struct iovec iv[2];
    int iv_count;
    int start_line;
    int content_length;
    Http::CHECK_STATE check_state;
    bool sseTag = false; // 判断是否是SSE连接，若是SSE连接，执行SSE线程
    bool webTag = false;
    std::string bas64;

};


#endif