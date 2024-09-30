#include "battle.h"

void Battle::battle_ready(void* arg) {
    Battle* bat = static_cast<Battle*>(arg);
    std::cout << "现在，战斗开始" << std::endl;
    bat->ready();
}

void Battle::ready() {
    for (auto &id:p1.skillIds) {
        Skill* s;
        if (SkillPool::skillT[id]->getInfo() == 1)
            s = new AtkSkill(*dynamic_cast<AtkSkill*>(SkillPool::skillT[id]));
        else
            s = new GainSkill(*dynamic_cast<GainSkill*>(SkillPool::skillT[id]));
        s->setTarget(&p1, &p2);
        batSkills.push_back(s);
    }

    for (auto &id:p2.skillIds) {
        Skill* s;
        if (SkillPool::skillT[id]->getInfo() == 1)
            s = new AtkSkill(*dynamic_cast<AtkSkill*>(SkillPool::skillT[id]));
        else
            s = new GainSkill(*dynamic_cast<GainSkill*>(SkillPool::skillT[id]));
        s->setTarget(&p2, &p1);
        batSkills.push_back(s);
    }
    fight();
}

void Battle::fight() {
    auto start = std::chrono::high_resolution_clock::now(); // 记录开始时间

    while (true) {
        auto end = std::chrono::high_resolution_clock::now(); // 记录结束时间
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        start = end;
        for (auto &it:batSkills) {
            if (it->curCd <= 0) {
                if (it->exec() == 0) {
                    it->curCd = it->cd;
                    std::cout << "执行技能" << std::endl;
                } else {
                    return;
                }
            } else {
                it->curCd -= duration.count();
            }
        }
        // std::cout << duration.count() << std::endl;
        // std::cout << duration << std::endl;
        // 模拟技能执行过程
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}
