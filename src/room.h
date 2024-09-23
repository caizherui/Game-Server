#ifndef ROOM_H
#define ROOM_H

#include "player.h"
#include <list>
#include <vector>


class Room {
public:
    Room();
    virtual ~Room() {}; 

    void add(Player* );
    void disOppo(); //  房间内分配对手

    static std::vector<Room *> roomLists;   //  对局房间

private:
    std::list<Player*> plsyerList;
    std::list<Player*> surviveList; //  存活用户列表
    std::list<Player*> restList;    //  休整用户列表
    std::list<Player*> updateList;  //  待更新用户列表
};


#endif