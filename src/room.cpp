#include "room.h"

std::unordered_map<int, Skill*> SkillPool::skillT;  // 用户和房间之间的关系

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

            // Battle* bat= new Battle(*surviveList[oppos[i]], *surviveList[oppos[i+1]]);
            // std::thread battle_thread(bat->battle_ready, (void *)bat);
            // battle_thread.detach();
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
    ++readyNum;
    lk.unlock();

    if (readyNum == surviveList.size()) {
        readyFlag = true;
        disOppo();
    }
}

std::list<int> Room::getOppoSkills(std::string userId) {
    for (auto it = surviveList.begin(); it != surviveList.end(); it++) {
        if ((*it)->playerId == userId) {
            return (*it)->oSkillIds;
        }
    }
}

int Room::getOppoHealth(std::string userId) {
    for (auto it = surviveList.begin(); it != surviveList.end(); it++) {
        if ((*it)->playerId == userId) {
            std::cout << (*it)->oMaxHealth << std::endl;
            return (*it)->oMaxHealth;
        }
    }
}

void Room::test() {
    Player* p1 = new Player("0");
    Player* p2 = new Player("1");
    std::list<int> s1 = {1000,1001,1002};
    std::list<int> s2 = {1000,1002,1003};
    p1->setInfo(std::move(s1), 120);
    p2->setInfo(std::move(s2), 120);
    surviveList.push_back(p1);
    surviveList.push_back(p2);

    SkillPool::skillT[1000] = new AtkSkill(6, 1, 3000000, 3000000);
    SkillPool::skillT[1001] = new GainSkill(1, 5000000, 5000000);
    SkillPool::skillT[1002] = new AtkSkill(5, 2, 5000000, 5000000);
    SkillPool::skillT[1003] = new AtkSkill(20, 1, 600000000, 0);

    Battle* bat= new Battle(*surviveList[0], *surviveList[1]);
    std::thread battle_thread(bat->battle_ready, (void *)bat);
    battle_thread.detach();
}