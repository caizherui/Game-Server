#ifndef PLAYER_H
#define PLAYER_H

#include <string>

class Player;

class Player {
public:
    Player(std::string);
    virtual ~Player() {}; 

    std::string playerId;
    int maxHealth;
    int gold;
    int expNums;
    int level;
    // Player oppo;    //  对局对手

    
    
};


#endif