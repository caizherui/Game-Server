#ifndef SkILL_H
#define SkILL_H

#include "player.h"
#include <json/json.h>  // 使用JsonCpp库来解析JSON
#include <chrono>
#include <cstdlib> // 包含 rand 和 srand
#include <ctime> // 包含 time

class Skill {
public: 
    Skill(const int && id, const int&& cd, const int &&curCd) : id(id), cd(cd), curCd(curCd) {};
    Skill(const Skill& s) : id(s.id), cd(s.cd), curCd(s.curCd) {};
    virtual int exec(std::string&) = 0;
    virtual void setTarget(std::shared_ptr<Player> own, std::shared_ptr<Player> oppo) = 0;
    virtual inline int getInfo() {return 0;};
    int id; //  技能编号
    int cd; //  技能cd
    float curCd;  //  当前技能cd
    std::shared_ptr<Player> src;
    std::shared_ptr<Player> target;

};

class AtkSkill : public Skill{
public:
    AtkSkill(const int &id, const int &damage, const int &fre, const int &cd, const int &curCd, const int &acc) : damage(damage), fre(fre), Skill(std::move(id), std::move(cd), std::move(curCd)), acc(acc) {std::cout << "技能1000填充完毕" << std::endl;};
    AtkSkill(const AtkSkill& s) : damage(s.damage), fre(s.fre), Skill(std::move(id), std::move(s.cd), std::move(s.curCd)), acc(s.acc)  {};
    AtkSkill* operator=(const AtkSkill* s) {
        damage = s->damage;
        fre = s->fre;
        id = s->id;
        cd = s->cd;
        curCd = curCd;
        acc = s->acc;
        return this;
    }
    virtual int exec(std::string&) override;
    virtual void setTarget(std::shared_ptr<Player> own, std::shared_ptr<Player> oppo) override;
    virtual inline int getInfo() override;

private:
    int damage;
    int fre;
    int acc;
};

class GainSkill : public Skill{
public:
    GainSkill(const int &id, const int &firm, const int& cd, const int &curCd, const int &sD, const int &sS) : firm(firm), sD(sD), sS(sS), Skill(std::move(id), std::move(cd), std::move(curCd)) {std::cout << "技能1001填充完毕" << std::endl;};
    GainSkill(const GainSkill& s) : firm(s.firm), sD(s.sD), sS(s.sS), Skill(std::move(s.id), std::move(s.cd), std::move(s.curCd)) {};
    GainSkill* operator=(const GainSkill* s) {
        firm = s->firm;
        sD = s->sD;
        sS = s->sS;
        id = s->id;
        cd = s->cd;
        curCd = curCd;
        return this;
    }
    virtual int exec(std::string&) override;
    virtual void setTarget(std::shared_ptr<Player> own, std::shared_ptr<Player> oppo) override;
    virtual inline int getInfo() override;

private:
    int firm;
    int sD;
    int sS;
};

class RedSkill : public Skill{
public:
    RedSkill(const int &id, const int &sand, const int& cd, const int &curCd) : sand(sand), Skill(std::move(id), std::move(cd), std::move(curCd)) {std::cout << "技能1001填充完毕" << std::endl;};
    RedSkill(const RedSkill& s) : sand(s.sand), Skill(std::move(s.id), std::move(s.cd), std::move(s.curCd)) {};
    RedSkill* operator=(const RedSkill* s) {
        sand = s->sand;
        id = s->id;
        cd = s->cd;
        curCd = curCd;
        return this;
    }
    virtual int exec(std::string&) override;
    virtual void setTarget(std::shared_ptr<Player> own, std::shared_ptr<Player> oppo) override;
    virtual inline int getInfo() override;

private:
    int sand;
};



#endif