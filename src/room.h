#ifndef ROOM_H
#define ROOM_H

#include <list>
#include <vector>
#include <unordered_map>
#include <random>
#include <algorithm>
#include <memory>
#include <mutex>
#include <thread>
#include "battle.h"


class Room {
public:
    Room();
    virtual ~Room() {}; 

    void add(Player* );
    void updatePlayer(std::string, std::list<int>&&, int, int); // 用户购物完毕，更新用户信息
    std::list<int> getOppoSkills(std::string); //  获取对手技能列表
    int getOppoHealth(std::string); //  获取对手最大生命
    void test();

    static std::vector<Room *> roomLists;   //  对局房间
    std::unordered_map<std::string, Player*> playerMap;  // 用户id和用户类之间的关系
    bool readyFlag = false;

private:
    void disOppo(); //  房间内分配对手

    std::vector<Player*> surviveList; //  存活用户列表
    int readyNum = 0; //    已经准备就绪的用户
    int battleNum = 0; //   已经进入战斗的用户，若battleNum和readyNum数量相等，则将readyNum置为空
    std::mutex mutexReady;   //  准备好的用户锁
    std::mutex mutexBattle;   //  准备好的用户锁

};


#endif