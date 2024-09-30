// #ifndef SSE_H
// #define SSE_H


// #include <iostream>
// #include <string>
// #include <map>
// #include <vector>
// #include <thread>
// #include <mutex>
// #include <sstream>
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <arpa/inet.h>
// #include <unistd.h>
// #include "room.h"

// class Sse {
// public:
//     // void push_event(const std::string&, const std::string&);
//     // void send_events(int);
//     // void sse_client(int);

//     void event_bind();
//     void send_events(int); 

//     static void event_listen(void*, int);
//     static void sse_client(void*, int);
    

//     static Sse* GetInstance() {
//         if (instance == nullptr)
//             instance = new Sse();
//         else
//             return instance;
//     }

// private:
//     static Sse* instance;   //  静态成员变量，类内声明，类外初始化
//     Sse() {};   //  Sse私有化
//     Sse(const Sse &) {};   //  拷贝构造函数私有化
//     Sse(const Sse &&) {};   //  移动构造函数私有化
//     Sse operator=(const Sse &) {};  //赋值构造函数私有化
// };

// #endif