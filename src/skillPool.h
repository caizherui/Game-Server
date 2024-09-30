#ifndef SkILL_POOL_H
#define SkILL_POOL_H

#include "skill.h"
#include <iostream>
#include <string>
#include <map>
#include <algorithm>
#include <unordered_map>

class SkillPool {
public:
    static std::unordered_map<int, Skill*> skillT;
    
};


#endif