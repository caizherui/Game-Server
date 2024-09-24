#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <list>
#include <iostream>
#include <condition_variable>
#include <mutex>

class Player;

class Player {
public:
    Player(std::string);

    // Player operator=(const Player& p) {
    //     maxHealth = p.maxHealth;
    //     skillIds = p.skillIds;
    // }
    virtual ~Player() {}; 

    std::string playerId;
    std::list<int> skillIds; 
    int maxHealth;
    int expNums;
    int level;
    std::list<int> oSkillIds; 
    int oMaxHealth; 

    void setInfo(std::list<int>&&, int); // 设置用户属性
    inline void setSkills(std::list<int>&&); //  设置技能表
    inline void setHealth(int);    //  设置最大生命值
    // Player oppo;    //  对局对手

    
    
};


#endif