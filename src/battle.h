#ifndef BATTLE_H
#define BATTLE_H


#include "skill.h"
#include "skillPool.h"
#include "http.h"
#include <thread>
#include <chrono>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <regex>
#include <string>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <sys/stat.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/uio.h>
#include <map>
#include <sstream>
#include <mysql/mysql.h>
#include <string>
#include <fstream>
#include <condition_variable>

#define DATA_LEVEL1   126  // data: 7+16 bit
#define DATA_LEVEL2   127  // data: 7+64 bit

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
    bool  m_Fin = true;         // 结尾标志位       1bit
    bool  m_Mask = false;        // 掩码处理         1bit
    char m_Opcode = 0x1;      // 数据类型         4bit
    int  m_LoadLen;     // Payload len      7bit
    int  m_Masking_Key = 0; // 
    long long  m_DataLength = 0;  // Ext payload len  16/64bit

    frame_head()
    {
        m_Fin = true;
        m_Mask = false;
        m_Opcode = 0x1;
        m_LoadLen = 0;
        m_DataLength = 0;
        m_Masking_Key = 0;
    }

    void clear()
    {
        m_Fin = true;
        m_Mask = false;
        m_Opcode = 0x1;
        m_LoadLen = 0;
        m_DataLength = 0;
        m_Masking_Key = 0;
    }
}tFrameHeader;


class Battle {
public:
    Battle(std::shared_ptr<Player> p1, std::shared_ptr<Player> p2) : p1(p1),p2(p2) {};
    virtual ~Battle() {};
    void ready();
    void fight();   //  战斗中，计算效果与伤害结算
    static void battle_ready(void*);    //  房间内玩家自走对战开始
    static std::unordered_map<std::string, int> webPlayer;  // websocket中，用户id和websocket连接fd的对应关系
    inline void sendWeb(int, std::string); //发送websocket请求

private:    
    void closeWebSocketConnection(int); // 关闭websocket连接
    std::vector<Skill*> batSkills;
    std::shared_ptr<Player> p1;
    std::shared_ptr<Player> p2;
    bool end;   // 游戏结束标志

    //  状态标志
    int state = 1000000;
    int maxState = 1000000;
};


#endif