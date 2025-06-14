#include <string>
#include<iostream>
#include "Gunner1.hpp"

Gunner1::Gunner1(int x, int y) : Player("play/PlayerBase.png","play/GunnerIcon.png","play/KnightIdle.png", x, y, 3, 13, 4, 2, 200,2, "Gunner1") {
    AddSkill({"Shoot", 2, 50, 30, 0.2});
    AddSkill({"Defend", 1, 0, 0, 0});
}
//SKILL:Name, Energy, Power, CritChance
bool Gunner1::Act(){
    
}

void Gunner1::chooseSkill(){}


