#include "websocket.h"

using namespace std;

bool MyWebsocket::Websocket_Server::TcpCreate()
{
    std::cout<<"Port:"<<m_Port<<",backlog:"<<m_Backlog<<endl;

    m_Sockfd = socket(PF_INET, SOCK_STREAM, 0);

    int opt = 1; 
    setsockopt(m_Sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));  
    struct sockaddr_in server_sockaddr;
    memset(&server_sockaddr, 0, sizeof(server_sockaddr));
    server_sockaddr.sin_family = PF_INET;
    server_sockaddr.sin_port = m_Port;
    server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    // bind
    bind(m_Sockfd, (struct sockaddr *)&server_sockaddr, sizeof(server_sockaddr));

    // listen
    listen(m_Sockfd, 5);

    // while (1) {
    //     int connfd = accept(listenfd, (struct sockaddr *)&client_address, &client_addrlength);
    //     // if (connfd < 0) {
    //     //     std::cout << "事件读取完毕" << std::endl;
    //     //     break;
    //     // }
    // }

    return true;
}

int MyWebsocket::Websocket_Server::Base64_encode(char* pSrcStr, int len, char* pResStr)
{
    BIO *b64, *bio;
    BUF_MEM *bptr = NULL;
    size_t size = 0;
 
    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);
 
    BIO_write(bio, pSrcStr, len);
    BIO_flush(bio);
 
    BIO_get_mem_ptr(bio, &bptr);
    memcpy(pResStr, bptr->data, bptr->length);
    pResStr[bptr->length] = '\0';
    size = bptr->length;
 
    BIO_free_all(bio);
    return size;
}

/*=============================================================== 
 * 函  数： Read_KeyBuff
 * 功  能： 读取key值
 * 参  数： pSrcStr         源字符串
 *          pResStr        结果字符串
 * 返回值： true            成功
 *         false           失败
 *=============================================================*/
bool MyWebsocket::Websocket_Server::Read_KeyBuff(char* pSrcStr, char* pResStr)
{
    if(NULL == pSrcStr)
    {
        return false;
    }

    int Key_End = -1;
    int Key_Begin = -1;
    int length = strlen(pSrcStr);       // 字符总长
    int step = strlen(WEBSOCKET_KEY);   // 关键字长度
    for(int i = 0; i < length - step; i ++)
    {
        if((':' == *(pSrcStr + step + i))&&(Key_Begin < 0))
        {
            Key_Begin = step + i + 1;
        }

        if(step + i + 2 <= length)
        {
            if(('\r' == *(pSrcStr + step + i + 1)) && ('\n' == *(pSrcStr + step + i + 2)))
            {
                Key_End = step + i;
                if(-1 == Key_Begin)
                {
                    return false;
                }
               break;
            }
        }
        else
        {
            // error data
            return false;
        }
    }
    memcpy(pResStr, pSrcStr + Key_Begin, Key_End - Key_Begin + 1);

    return true;
}

/*=============================================================== 
 * 函  数： SendFrameHead
 * 功  能： 根据自定义结构体，发送数据头
 * 参  数： ptFrameHeader     自定义结构体
 * 返回值： true              成功
 *         false             失败
 *=============================================================*/
bool MyWebsocket::Websocket_Server::SendFrameHead(int fd, tFrameHeader* ptFrameHeader)
{
    bool Res = false;
    if(NULL == ptFrameHeader || fd < 0)
    {
        return Res;
    }
    
    uchar* pHead = NULL;
    uchar byte1 = ptFrameHeader->m_Opcode;
    uchar byte2 = ptFrameHeader->m_LoadLen;
    int Length = 0;

    if(true == ptFrameHeader->m_Fin)
    {
        byte1 += 0x80;
    }

    if(true == ptFrameHeader->m_Mask)
    {
        byte2 += 0x80;
    }

    if(ptFrameHeader->m_LoadLen == DATA_LEVEL1)
    {
        // 7 + 16 bit
        pHead = new uchar(4);
        pHead[0] = byte1;
        pHead[1] = byte2;
        pHead[2] = (ptFrameHeader->m_DataLength>>8)&0xff;
        pHead[3] = (ptFrameHeader->m_DataLength& 0xff);
        Length = 4;
    }
    else if (ptFrameHeader->m_LoadLen == DATA_LEVEL2)
    {
        // 7 + 64 bit
        pHead = new uchar(10);
        pHead[0] = byte1;
        pHead[1] = byte2;
        ull temp = ptFrameHeader->m_DataLength;
        for(int i = 0; i < 8; i ++)
        {
            pHead[2 + i] = ( ptFrameHeader->m_DataLength>>(8 * (7 - i) ) )&0xff;
        }
        Length = 12;
    }
    else if((ptFrameHeader->m_LoadLen > 0)&&(ptFrameHeader->m_LoadLen < DATA_LEVEL1))
    {
        pHead = new uchar(2);
        pHead[0] = byte1;
        pHead[1] = byte2;
        Length = 2;
    }
    else
    {
        std::cout<<"error in data"<<endl;
        return false;
    }

    if(send(fd, pHead, Length, 0) < 1)
    {
        perror("send");
        Res = false;
    }

    if(NULL != pHead)
    {
        delete [] pHead;
    }
    
    return true;
}


/*=============================================================== 
 * 函  数： Shake_Hand
 * 功  能： websocket握手
 * 参  数： pKey             指向"Sec-WebSocket-Key"开头的buf
 * 返回值： true             成功
 *         false            失败
 *=============================================================*/
bool MyWebsocket::Websocket_Server::Shake_Hand(int fd, char* pKey)
{
    char SHA1_data[SHA1_LEN + 1];
    char Frame_Head[BUFF_SIZE];
    char Sec_Accept[32];
    memset(SHA1_data, 0, sizeof(SHA1_data));
    memset(Frame_Head, 0, sizeof(Frame_Head));

    // 获取 Sec-WebSocket-Key 的值
    char WSkey[64];
    memset(WSkey, 0, sizeof(WSkey));
    Read_KeyBuff(pKey, WSkey);

    strcat(WSkey, DECODE_STRING);
    SHA1((uchar*)&WSkey + 19, strlen(WSkey + 19), (uchar*)&SHA1_data);
    Base64_encode(SHA1_data, strlen(SHA1_data), Sec_Accept);

    sprintf(Frame_Head, "HTTP/1.1 101 Switching Protocols\r\n" \
                                "Upgrade: websocket\r\n" \
                                "Connection: Upgrade\r\n" \
                                "Sec-WebSocket-Accept:%s\r\n" \
                                "\r\n", Sec_Accept);

    std::cout<<"Response:"<<endl;
    std::cout<<Frame_Head<<endl;

    int Res = write(fd, Frame_Head, sizeof(Frame_Head));
    if(Res < 0)
    {
        perror("send");
        return false;
    }
    
    m_ClientNum ++;
    return true;
}


