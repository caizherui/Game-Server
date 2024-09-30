#ifndef WEBSOCKET_H
#define WEBSOCKET_H

#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <openssl/sha.h>
#include <openssl/pem.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <unistd.h>
#include <string.h>
#include <queue>
#include <mutex>
#include <thread>
#include <atomic>

namespace MyWebsocket
{

#define DECODE_STRING "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"
#define WEBSOCKET_KEY "Sec-WebSocket-Key"
#define PORT_MAX      65535
#define PORT_MIN      1024
#define BUFF_SIZE     1024
#define DATA_SIZE     1024*1024  // 1mb
#define SHA1_LEN      20
#define DATA_LEVEL1   126  // data: 7+16 bit
#define DATA_LEVEL2   127  // data: 7+64 bit
#define READFD_NUM    20

typedef unsigned char uchar;
typedef unsigned int  uint;
typedef unsigned short ushort;
typedef unsigned long long ull;

// 数据类型
enum OpCode
{
    CODE_ADD   = 0x0,  // 附加帧
    CODE_TEXT  = 0x1,  // 文本帧
    CODE_BIN   = 0x2,  // 二进制帧
    CODE_CLOSE = 0x8,  // 关闭
    CODE_PING  = 0x9,  // ping(心跳) 
    CODE_PONG  = 0xA,  // pong(心跳)
    CODE_UNKNOWN = 0xF // 未知
};

// 消息头
typedef struct frame_head
{
    bool  m_Fin;         // 结尾标志位       1bit
    bool  m_Mask;        // 掩码处理         1bit
    uchar m_Opcode;      // 数据类型         4bit
    uint  m_LoadLen;     // Payload len      7bit
    uint  m_Masking_Key; // 
    ull   m_DataLength;  // Ext payload len  16/64bit

    frame_head()
    {
        m_Fin = true;
        m_Mask = true;
        m_Opcode = 0xf;
        m_LoadLen = 0;
        m_DataLength = 0;
        m_Masking_Key = 0;
    }

    void clear()
    {
        m_Fin = true;
        m_Mask = true;
        m_Opcode = 0xf;
        m_LoadLen = 0;
        m_DataLength = 0;
        m_Masking_Key = 0;
    }
}tFrameHeader;

class Websocket_Server
{
public:
    int  m_Port = 8080;      // 端口号
    int  m_Backlog;   // 未完成连接队列和已完成连接队列的总和
    int  m_Sockfd;    // 套接字  
    int  m_Clientfd;
    bool m_bShake;    // 握手成功标志位
    std::atomic<int> m_ClientNum;     // 连接的客户端数量
    std::atomic<bool>   m_bStopTask;  // 停止
    std::atomic<char* > m_pSendBuff;

    std::queue<int> m_Data;
    std::mutex   m_DataMutex;
    std::thread* m_pTasker;

    
    int m_ArrWebClient[READFD_NUM];

    bool SendFrameHead(int fd, tFrameHeader* ptFrameHeader);
    bool FrameHead_Send(int fd, bool fin, OpCode type, ull length);  
    void Run();

    // about the beginning of websocket
    int DataRecv(int fd);
    bool TcpCreate();
    bool Shake_Hand(int fd, char* pKey);
    bool Read_KeyBuff(char* pSrcStr, char* pResStr);
    void SendData();
    bool RecvFrameHead(tFrameHeader* ptFrameHeader);
    int  Base64_encode(char* pSrcStr, int len, char* pResStr);

    Websocket_Server() {};
    ~Websocket_Server() {};

    // api for user
    // int GetClientNum();      // 获取客户端连接的数量
    // bool Start_Task();       // 开启线程
    bool SetData(int data);  // 写入数据
};

} // namespace MyWebsocket

#endif
