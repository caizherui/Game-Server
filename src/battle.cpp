#include "battle.h"

const char *CLOSE_FRAME = "\x88\x02\x03\xe8"; // WebSocket关闭帧

void Battle::battle_ready(void* arg) {
    Battle* bat = static_cast<Battle*>(arg);
    std::cout << "现在，战斗开始" << std::endl;
    bat->ready();
}

void Battle::ready() {
    std::cout << "进入战斗阶段" << std::endl;
    for (auto &id:p1->skillIds) {
        Skill* s = nullptr;
        if (SkillPool::skillT[id]->getInfo() == 1)
            s = new AtkSkill(*dynamic_cast<AtkSkill*>(SkillPool::skillT[id]));
        else if (SkillPool::skillT[id]->getInfo() == 2)
            s = new GainSkill(*dynamic_cast<GainSkill*>(SkillPool::skillT[id]));
        else
            s = new RedSkill(*dynamic_cast<RedSkill*>(SkillPool::skillT[id]));
        s->id = SkillPool::skillT[id]->id;
        s->setTarget(p1, p2);
        batSkills.push_back(s);
    }

    for (auto &id:p2->skillIds) {
        Skill* s = nullptr;
        if (SkillPool::skillT[id]->getInfo() == 1)
            s = new AtkSkill(*dynamic_cast<AtkSkill*>(SkillPool::skillT[id]));
        else if (SkillPool::skillT[id]->getInfo() == 2)
            s = new GainSkill(*dynamic_cast<GainSkill*>(SkillPool::skillT[id]));
        else
            s = new RedSkill(*dynamic_cast<RedSkill*>(SkillPool::skillT[id]));
        s->id = SkillPool::skillT[id]->id;    
        s->setTarget(p2, p1);
        batSkills.push_back(s);
    }
    fight();
}

void Battle::fight() {
    auto start = std::chrono::high_resolution_clock::now(); // 记录开始时间

    while (true) {
        auto end = std::chrono::high_resolution_clock::now(); // 记录结束时间
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        start = end;
        //  执行状态栏效果
        if (state <= 0) {
            if (p1->sS == 1) {  //  剑灵一阶段
                p2->health -= 2;
            }
            if (p2->sS == 1) {
                p1->health -= 2;
            }
            state = maxState;
        } else {
            state -= duration.count();
        }
        for (auto &it:batSkills) {
            std::string request_json = "";
            if (it->curCd <= 0) {
                std::cout << "充能完毕" << it->id << std::endl; 
                std::cout << request_json << std::endl; 
                if (it->exec(request_json) == 0) {
                    std::cout << "发送历练消息" << std::endl; 
                    if (p1->playerId != "-1")
                        sendWeb(Battle::webPlayer[p1->playerId], request_json);
                    if (p2->playerId != "-1")  
                        sendWeb(Battle::webPlayer[p2->playerId], request_json); 
                    std::cout << "历练消息发送完毕" << std::endl; 
                    it->curCd = it->cd;
                } else{
                    int pos = request_json.find(' ');
                    std::string pId = request_json.substr(pos+1);
                    pos = pId.find(' ');
                    pId = pId.substr(0, pos);
                    std::cout << "用户1的历练值：" << p1->expNums << "用户2的历练值：" << p2->expNums << std::endl;
                    if (pId != "-1") {
                        if (pId == p1->playerId && p1->expNums == 1) {
                            std::cout << Room::roomLists[Http::userRoomMap[p1->playerId]]->surviveList.size() << " " << pId << std::endl;
                            for (int i = 0; i < Room::roomLists[Http::userRoomMap[p1->playerId]]->surviveList.size(); i++) {
                                std::cout << Room::roomLists[Http::userRoomMap[p1->playerId]]->surviveList[i]->playerId << " " << pId << std::endl;
                                if (Room::roomLists[Http::userRoomMap[p1->playerId]]->surviveList[i]->playerId == pId) {
                                    auto it = Room::roomLists[Http::userRoomMap[p1->playerId]]->surviveList.begin()+i;
                                    Room::roomLists[Http::userRoomMap[p1->playerId]]->surviveList.erase(it);
                                    std::cout << "删除成功" << std::endl;
                                }
                            }
                            std::cout << "用户结束试炼" << std::endl;
                        } else if(p2->expNums == 1){
                            std::cout << Room::roomLists[Http::userRoomMap[p1->playerId]]->surviveList.size() << " " << pId << std::endl;
                            for (int i = 0; i < Room::roomLists[Http::userRoomMap[p2->playerId]]->surviveList.size(); i++) {
                                std::cout << Room::roomLists[Http::userRoomMap[p1->playerId]]->surviveList[i]->playerId << " " << pId << std::endl;
                                if (Room::roomLists[Http::userRoomMap[p2->playerId]]->surviveList[i]->playerId == pId) {
                                    auto it = Room::roomLists[Http::userRoomMap[p2->playerId]]->surviveList.begin()+i;
                                    Room::roomLists[Http::userRoomMap[p2->playerId]]->surviveList.erase(it);
                                    std::cout << "删除成功" << std::endl;
                                }
                            }
                            std::cout << "用户结束试炼" << std::endl;
                        }
                    }
                    if (p1->playerId != "-1")
                        sendWeb(Battle::webPlayer[p1->playerId], request_json);
                    if (p2->playerId != "-1")
                        sendWeb(Battle::webPlayer[p2->playerId], request_json);
                    if (p1->playerId != "-1")
                        closeWebSocketConnection(Battle::webPlayer[p1->playerId]);
                    if (p2->playerId != "-1")
                        closeWebSocketConnection(Battle::webPlayer[p2->playerId]);
                    std::cout << "历练结束，关闭websocket连接" << std::endl;
                    return;
                }
            } else {
                it->curCd -= duration.count();
            }
        }
        // std::cout << duration.count() << std::endl;
        // std::cout << duration << std::endl;
        // 模拟技能执行过程
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void Battle::sendWeb(int fd, std::string message) {
    char* payloadData = const_cast<char*>(message.c_str());
    size_t payloadLength = strlen(payloadData);

    tFrameHeader* ptFrameHeader = new tFrameHeader();
    // 封装websocket数据帧
    std::cout << "数据长度" << payloadLength << std::endl;
    
    char* pHead = NULL;
    ptFrameHeader->m_Opcode = 0x01;
    // if (payloadLength > 15)
    //     ptFrameHeader->m_LoadLen = 126;
    // else 
    ptFrameHeader->m_LoadLen = payloadLength;

    char byte1 = 0x81;
    char byte2 = ptFrameHeader->m_LoadLen;
    int Length = 0;

    if(ptFrameHeader->m_LoadLen == DATA_LEVEL1) {
        // 7 + 16 bit
        pHead = new char(4);
        pHead[0] = byte1;
        pHead[1] = byte2;
        pHead[2] = (ptFrameHeader->m_DataLength>>8)&0xff;
        pHead[3] = (ptFrameHeader->m_DataLength& 0xff);
        Length = 4;
    }
    else if (ptFrameHeader->m_LoadLen == DATA_LEVEL2) {
        // 7 + 64 bit
        pHead = new char(10);
        pHead[0] = byte1;
        pHead[1] = byte2;
        long long temp = ptFrameHeader->m_DataLength;
        for(int i = 0; i < 8; i ++)
        {
            pHead[2 + i] = ( ptFrameHeader->m_DataLength>>(8 * (7 - i) ) )&0xff;
        }
        Length = 10;
    }
    else {
        pHead = new char(2);
        pHead[0] = byte1;
        pHead[1] = byte2;
        Length = 2;
    }

    // char* frameData = new char[Length+payloadLength];

    // memcpy(frameData, pHead, Length);
    // memcpy(frameData + Length, payloadData, payloadLength);
    //  memset(frameData + Length + payloadLength, '\0', 1);
    // std::cout << frameData << std::endl;
    std::cout << "拂晓之前" << fd << std::endl;

    memcpy(pHead+Length, payloadData, payloadLength);
    ssize_t se = send(fd, pHead, Length+payloadLength, 0);
    // if (bytes_sent == -1) {
    //     std::cout << "Error sending HTTP response: " << std::endl;
    // } else {
    //     std::cout << "HTTP response sent successfully." << std::endl;
    // }

    delete [] pHead;
    delete ptFrameHeader;

    std::cout << "发送websocket请求" << std::endl;

    // websocket
    // ssize_t bytes_sent = send(fd, response_post.c_str(), response_post.size(), 0);
    // if (bytes_sent == -1) {
    //     std::cerr << "Error sending HTTP response: " << std::strerror(errno) << std::endl;
    // } else if (bytes_sent < static_cast<ssize_t>(response_post.size())) {
    //     std::cerr << "Partial send: Sent " << bytes_sent << " out of " << response_post.size() << " bytes." << std::endl;
    // } else {
    //     std::cout << "HTTP response sent successfully." << std::endl;
    // }
}

// 关闭连接
void Battle::closeWebSocketConnection(int sockfd) {
    // 发送关闭帧
    if(send(sockfd, CLOSE_FRAME, strlen(CLOSE_FRAME), 0) == -1) {
        perror("Error sending close frame");
        // 可以选择在此处处理错误或退出程序
        return;
    }

    // 等待服务器确认关闭
    char buffer[1024] = {0};
    ssize_t bytesRead = recv(sockfd, buffer, sizeof(buffer) - 1, 0);

    // 检查是否收到了关闭帧
    // if (bytesRead >= 2 && buffer[0] == 0x88 && buffer[1] == 0x02) {
    //     std::cout << "Received close confirmation from server." << std::endl;
    // } else {
    //     std::cout << "Did not receive expected close confirmation from server." << std::endl;
    // }

    if (bytesRead >= 1) {
        std::cout << "Received close confirmation from server." << std::endl;
    } else {
        std::cout << "Did not receive expected close confirmation from server." << std::endl;
    }

    // 关闭套接字
    // close(sockfd);
}