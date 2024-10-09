#include "player.h"

Player::Player(std::string id) {
    playerId = id;
    expNums = 2;
    maxHealth = 100;
    level = 1;
    health = maxHealth;
}

void Player::setInfo(std::list<int> &&l, int m, int expNums) {
    setSkills(std::move(l));
    setHealth(m);
    this->expNums = expNums;
    initState();
}

void Player::setSkills(std::list<int> &&l) {
    skillIds = std::move(l);
}

void Player::setHealth(int m) {
    health = m;
    maxHealth = m;
}

void Player::initState() {
    sand = 0;
    firm = 0;
    sD = 0;
    sS = 0;
}

void Player::rest() {
    std::cout << "用户进入休整状态" << std::endl;
}