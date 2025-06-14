#include <string>
#include<iostream>
#include "Knight1.hpp"

Knight1::Knight1(int x, int y) : Player("play/PlayerBase.png","play/ShieldIcon.png","play/KnightIdle.png", x, y, 100, 16.5, 20, 2, 100, 1, "Knight_00") {
    AddSkill({"Blade", 1, 10, 100, 0.2});
    AddSkill({"Defend", 1, 0, 0, 0});
}
//SKILL:Name, Energy, Power, CritChance
bool Knight1::Act(){
    
}

void Knight1::chooseSkill(){}


