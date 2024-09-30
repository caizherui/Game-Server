#ifndef BATTLE_H
#define BATTLE_H


#include "skill.h"
#include "skillPool.h"
#include <thread>
#include <chrono>


class Battle {
public:
    Battle(Player& p1, Player& p2) : p1(p1),p2(p2) {};
    virtual ~Battle() {};
    void ready();
    void fight();   //  战斗中，计算效果与伤害结算
    static void battle_ready(void*);    //  房间内玩家自走对战开始

private:
    std::vector<Skill*> batSkills;
    Player p1;
    Player p2;
    bool end;   // 游戏结束标志


};


#endif