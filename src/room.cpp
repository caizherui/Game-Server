#include "room.h"

std::unordered_map<int, Skill*> SkillPool::skillT;  // 用户和房间之间的关系

Room::Room() {
    
}

void Room::add(std::shared_ptr<Player> player) {
    std::cout << "新增了一个用户" << std::endl;
    surviveList.push_back(player);
}

void Room::disOppo() {  //  房间内分配对手
    //  技能池
    SkillPool::skillT[1000] = new AtkSkill(1000, 6, 1, 3000000, 3000000, 0);
    SkillPool::skillT[1001] = new GainSkill(1001, 1, 600000000, 0, 0, 0);
    SkillPool::skillT[1002] = new AtkSkill(1002, 5, 2, 5000000, 5000000, 0);
    SkillPool::skillT[1003] = new AtkSkill(1003, 20, 1, 600000000, 0, 0);
    SkillPool::skillT[1004] = new AtkSkill(1004, 5, 1, 3000000, 3000000, 1);
    SkillPool::skillT[1005] = new RedSkill(1005, 2, 600000000, 0);
    SkillPool::skillT[1006] = new GainSkill(1006, 0, 600000000, 0, 0, 1);
    SkillPool::skillT[1007] = new GainSkill(1007, 0, 600000000, 0, 1, 0);

    // if (readyNum == 1) {    //  历练结束，用户胜利结算
    //     Battle::webPlayer[userId]
    // }

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
            std::cout << "对手技能数量" << surviveList[oppos[i+1]]->skillIds.size() << std::endl;
            surviveList[oppos[i]]->oMaxHealth = surviveList[oppos[i+1]]->maxHealth;
            surviveList[oppos[i+1]]->oSkillIds = surviveList[oppos[i]]->skillIds;
            std::cout << "对手技能数量" << surviveList[oppos[i]]->skillIds.size() << std::endl;
            surviveList[oppos[i+1]]->oMaxHealth = surviveList[oppos[i]]->maxHealth;

            std::cout << "用户" << surviveList[oppos[i]]->playerId << "和用户" << surviveList[oppos[i+1]]->playerId << "对局建立完毕" << std::endl;
            ++i;
            ++i;
        }
    } else {    //  奇数个玩家存活，此时会有镜像对局
        // 使用随机设备初始化随机数生成器
        std::random_device rd;
        std::mt19937 gen(rd());
        std::vector<int> oppos;
        for (int i = 0; i < readyNum; i++) {
            oppos.push_back(i);
        }
        std::shuffle(oppos.begin(), oppos.end(), gen);
        int i = 0;
        while (i < readyNum-1) {
            surviveList[oppos[i]]->oSkillIds = surviveList[oppos[i+1]]->skillIds;
            std::cout << "对手技能数量" << surviveList[oppos[i+1]]->skillIds.size() << std::endl;
            surviveList[oppos[i]]->oMaxHealth = surviveList[oppos[i+1]]->maxHealth;
            surviveList[oppos[i+1]]->oSkillIds = surviveList[oppos[i]]->skillIds;
            std::cout << "对手技能数量" << surviveList[oppos[i]]->skillIds.size() << std::endl;
            surviveList[oppos[i+1]]->oMaxHealth = surviveList[oppos[i]]->maxHealth;

            std::cout << "用户" << surviveList[oppos[i]]->playerId << "和用户" << surviveList[oppos[i+1]]->playerId << "对局建立完毕" << std::endl;
            ++i;
            ++i;
        }
        surviveList[oppos[i]]->oSkillIds = surviveList[oppos[0]]->skillIds;
        surviveList[oppos[i]]->oMaxHealth = surviveList[oppos[0]]->maxHealth;
    }
}

void Room::updatePlayer(std::string userId, std::list<int>&& skillIds, int maxHealth, int expNums) {
    // if (expNums == 0) {
    //     for (auto it = surviveList.begin(); it != surviveList.end(); it++) {
    //         std::cout << "执行删除用户操作" << std::endl;
    //         if ((*it)->playerId == userId) {
    //             surviveList.erase(it);
    //             if (readyNum == surviveList.size()) {
    //                 readyFlag = true;
    //                 std::unique_lock<std::mutex> lk(mutexBattle);
    //                 // battleNum = readyNum;
    //                 lk.unlock();
    //                 disoppo();
    //             }
    //             return;
    //         }
    //     }
    // }
    for (auto &it:surviveList) {
        if (it->playerId == userId)
            it->setInfo(std::move(skillIds), maxHealth, expNums);
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
        std::cout << (*it)->playerId << std::endl;
        if ((*it)->playerId == userId) {
            std::cout << "技能数量" << (*it)->oSkillIds.size() << std::endl;
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

void Room::joinBattle() {
    std::unique_lock<std::mutex> lk(mutexBattle);
    battleNum++;
    lk.unlock();
    if (battleNum == readyNum) {
        std::cout << "全员集结，历练开始" << std::endl;
        int i = 0;
        if (battleNum % 2 == 0) {   //  偶数位玩家
            while (i < battleNum) {
                Battle* bat= new Battle(surviveList[i], surviveList[i+1]);
                std::thread battle_thread(bat->battle_ready, (void *)bat);
                battle_thread.detach();
                ++i;
                ++i;
            }
        } else {    //  奇数位玩家，需要创建镜像
            while (i < battleNum-1) {
                Battle* bat= new Battle(surviveList[i], surviveList[i+1]);
                std::thread battle_thread(bat->battle_ready, (void *)bat);
                battle_thread.detach();
                ++i;
                ++i;
                std::cout << "创建普通对局" << std::endl;
            }
            std::cout << "创建镜像对局" << std::endl;
            std::shared_ptr<Player> mirror = std::make_shared<Player>("65536");
            mirror->setSkills(surviveList[0]->skillIds);
            mirror->setHealth(surviveList[0]->maxHealth);
            Battle* bat= new Battle(surviveList[i], mirror);
            std::thread battle_thread(bat->battle_ready, (void *)bat);
            battle_thread.detach();
        }
        webReuse.clear();
        readyNum = 0;
        battleNum = 0;
        readyFlag = false;
    }
}

void Room::test() {
    // Player* p1 = new Player("2");
    // Player* p2 = new Player("3");
    // std::list<int> s1 = {1000,1002};
    // std::list<int> s2 = {1000,1002,1003};
    // p1->setInfo(std::move(s1), 120);
    // p2->setInfo(std::move(s2), 120);
    // surviveList.push_back(p1);
    // surviveList.push_back(p2);

    // SkillPool::skillT[1000] = new AtkSkill(1000, 6, 1, 3000000, 3000000);
    // // SkillPool::skillT[1001] = new GainSkill(1001, 1, 5000000, 5000000);
    // SkillPool::skillT[1002] = new AtkSkill(1002, 5, 2, 5000000, 5000000);
    // SkillPool::skillT[1003] = new AtkSkill(1003, 20, 1, 600000000, 0);

    // Battle* bat= new Battle(*surviveList[0], *surviveList[1]);
    // std::thread battle_thread(bat->battle_ready, (void *)bat);
    // battle_thread.join();
}