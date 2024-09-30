#include "player.h"

Player::Player(std::string id) {
    playerId = id;
    expNums = 3;
    maxHealth = 100;
    level = 1;
    health = maxHealth;
}

void Player::setInfo(std::list<int> &&l, int m) {
    setSkills(std::move(l));
    setHealth(m);
}

inline void Player::setSkills(std::list<int> &&l) {
    skillIds = std::move(l);
}

inline void Player::setHealth(int m) {
    maxHealth = m;
}

void Player::rest() {
    std::cout << "用户进入休整状态" << std::endl;
}