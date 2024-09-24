#include "room.h"

Room::Room() {
    
}

void Room::add(Player* player) {
    surviveList.push_back(player);
}

void Room::disOppo() {  //  房间内分配对手
    if (readyNum % 2 == 0) {
        // 使用随机设备初始化随机数生成器
        std::random_device rd;
        std::mt19937 gen(rd());
        std::vector<int> oppos;
        for (int i = 0; i < readyNum; i++) {
            oppos.push_back(i);
        }
        std::shuffle(oppos.begin(), oppos.end(), gen);
        int i = 0;
        while (i < readyNum) {
            surviveList[oppos[i]]->oSkillIds = surviveList[oppos[i+1]]->skillIds;
            surviveList[oppos[i]]->oMaxHealth = surviveList[oppos[i+1]]->maxHealth;
            surviveList[oppos[i+1]]->oSkillIds = surviveList[oppos[i]]->skillIds;
            surviveList[oppos[i+1]]->oMaxHealth = surviveList[oppos[i]]->maxHealth;
            ++i;
            ++i;
        }
    } else {
        
    }
}

void Room::updatePlayer(std::string userId, std::list<int>&& skillIds, int maxHealth, int expNums) {
    if (expNums == 0) {
        for (auto it = surviveList.begin(); it != surviveList.end(); it++) {
            if ((*it)->playerId == userId) {
                surviveList.erase(it);
                if (readyNum == surviveList.size()) {
                    readyFlag = true;
                    std::unique_lock<std::mutex> lk(mutexBattle);
                    battleNum = readyNum;
                    lk.unlock();
                }
                return;
            }
        }
    }
    for (auto &it:surviveList) {
        if (it->playerId == userId)
            it->setInfo(std::move(skillIds), maxHealth);
    }

    std::unique_lock<std::mutex> lk(mutexReady);
    readyNum++;
    lk.unlock();

    if (readyNum == surviveList.size()) {
        readyFlag = true;
        disOppo();
    }
}