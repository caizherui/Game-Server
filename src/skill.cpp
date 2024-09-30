#include "skill.h"

int AtkSkill::exec() {
    target->health -= damage*fre;
    if (target->health <= 0) {
        target->rest();
        return 1;
    }
    return 0;
}

int GainSkill::exec() {
    target += firm;
    return 0;
}

void AtkSkill::setTarget(Player* own, Player* oppo) {
    this->src = own;
    this->target = oppo;
}

void GainSkill::setTarget(Player* own, Player* oppo) {
    this->src = own;
    this->target = own;
}

int AtkSkill::getInfo() {
    return 1;
}

int GainSkill::getInfo() {
    return 2;
}