#include <string>
#include<iostream>
#include "Turret1.hpp"

Knight1::Knight1(int x, int y) : Turret("play/Knight_03.png", x, y, 10, 16.5, 20, 2, 100, "Knight_00") {
    AddSkill({"Blade", 1, 10, 100, 0.2});
    AddSkill({"Defend", 1, 5, 0, 0});
}
//SKILL:Name, Energy, Power, CritChance
bool Knight1::Act(){
    
}

void Knight1::chooseSkill(){}

