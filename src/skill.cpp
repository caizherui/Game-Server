#include "skill.h"

int AtkSkill::exec(std::string& message) {
    std::cout << "目标当前生命" << target->health << std::endl;
    int hurt;
    // 使用当前时间作为随机数种子
    std::srand(std::time(nullptr));
    // 生成1到20之间的随机数
    int random_number = std::rand() % 20 + 1;
    if (src->sand >= random_number) {
        hurt = 0;
    } else {
        if (acc == 0)
            hurt = (damage+src->sD-target->firm)*fre;
        else
            hurt = (damage+src->sD)*fre;
    }
    target->health -= hurt;
    std::string ide = "0000";
    message = ide + " " + std::to_string(id)+" "+src->playerId+" "+std::to_string(src->health)+" "+std::to_string(target->health);
    std::cout << message << std::endl; 
    if (target->health <= 0) {
        target->rest();
        message = "gameEnd " + target->playerId + " end";
        return 1;
    }
    return 0;
}

int GainSkill::exec(std::string& message) {
    target->firm += firm;
    target->sD += sD;
    target->sS += sS;
    std::string ide = "0001";
    message = ide + " " + std::to_string(id)+" "+src->playerId+" "+std::to_string(firm)+" "+std::to_string(sD) + " " + std::to_string(sS);
    return 0;
}

int RedSkill::exec(std::string& message) {
    target->sand += sand;
    std::string ide = "0002";
    message = ide + " " + std::to_string(id)+" "+src->playerId+" "+std::to_string(src->health)+" "+std::to_string(target->health);
    return 0;
}

void AtkSkill::setTarget(std::shared_ptr<Player> own, std::shared_ptr<Player> oppo) {
    this->src = own;
    this->target = oppo;
}

void GainSkill::setTarget(std::shared_ptr<Player> own, std::shared_ptr<Player> oppo) {
    this->src = own;
    this->target = own;
}

void RedSkill::setTarget(std::shared_ptr<Player> own, std::shared_ptr<Player> oppo) {
    this->src = own;
    this->target = oppo;
}

int AtkSkill::getInfo() {
    return 1;
}

int GainSkill::getInfo() {
    return 2;
}

int RedSkill::getInfo() {
    return 3;
}