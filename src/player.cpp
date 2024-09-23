#include "player.h"

Player::Player(std::string id) {
    playerId = id;
    expNums = 3;
    maxHealth = 100;
    gold = 12;
    level = 1;
}