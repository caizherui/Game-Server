#include "room.h"

Room::Room() {
    
}

void Room::add(Player* player) {
    plsyerList.push_back(player);
    surviveList.push_back(player);
}

void Room::disOppo() {  //  房间内分配对手
    
}