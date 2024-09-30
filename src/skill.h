#ifndef SkILL_H
#define SkILL_H

#include "player.h"

class Skill {
public: 
    Skill(const int&& cd, const int &&curCd) : cd(cd), curCd(curCd) {};
    Skill(const Skill& s) : cd(s.cd), curCd(s.curCd) {};
    virtual int exec() = 0;
    virtual void setTarget(Player* own, Player* oppo) = 0;
    virtual inline int getInfo() {return 0;};
    int cd; //  技能cd
    float curCd;  //  当前技能cd
    Player* src;
    Player* target;

};

class AtkSkill : public Skill{
public:
    AtkSkill(const int &damage, const int &fre, const int &cd, const int &curCd) : damage(damage), fre(fre), Skill(std::move(cd), std::move(curCd)) {std::cout << "技能1000填充完毕" << std::endl;};
    AtkSkill(const AtkSkill& s) : damage(s.damage), fre(s.fre), Skill(std::move(s.cd), std::move(s.curCd))  {};
    virtual int exec() override;
    virtual void setTarget(Player* own, Player* oppo) override;
    virtual inline int getInfo() override;

private:
    int damage;
    int fre;
};

class GainSkill : public Skill{
public:
    GainSkill(const int &firm, const int& cd, const int &curCd) : firm(firm), Skill(std::move(cd), std::move(curCd)) {std::cout << "技能1001填充完毕" << std::endl;};
    GainSkill(const GainSkill& s) : firm(s.firm), Skill(std::move(s.cd), std::move(s.curCd)) {};
    virtual int exec() override;
    virtual void setTarget(Player* own, Player* oppo) override;
    virtual inline int getInfo() override;

private:
    int firm;
};



#endif