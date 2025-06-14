#include <string>
#include<iostream>
#include "EnemyGunner1.hpp"
#include"Scene/PlayScene.hpp"
#include"Engine/IntPoint.hpp"

EnemyGunner1::EnemyGunner1(int x, int y) : Enemy("play/EnemyBase.png","play/GunnerIcon.png","play/KnightIdle.png", x, y, 10, 10, 5, 2, 200, 2, "Knight_01") {
    AddSkill({"Shoot", 2, 50, 5, 0.2});
    AddSkill({"Defense", 1, 0, 0, 0});
    attackRange=2;
}

//SKILL:Name,Range, Energy, Power, CritChance
bool EnemyGunner1::Act(){
    int closetDist=10000;
    int bestStep=10000;
    Engine::IntPoint bestMove=gridPos;
    for(auto& r:radius){
        if(!MoveValid[r]) continue;
        //cout<<radiusStep[r]<<" "<<distance<<endl;
        for(auto obj:getPlayScene()->UnitGroup->GetObjects()){
            Unit* unit = dynamic_cast<Unit*>(obj);
            if(unit->IsPlayer()){
                int dist = abs(r.x - unit->gridPos.x)+abs(r.y - unit->gridPos.y);
                if(dist<closetDist&&dist>=attackRange){
                    target = unit;
                    closetDist=dist;
                    bestMove=r;
                    bestStep=radiusStep[r];
                }
                else if(dist==closetDist&&radiusStep[r]<bestStep){
                    target = unit;
                    bestMove=r;
                    bestStep=radiusStep[r];
                }
            }
            
        }
    }
    if(closetDist<=attackRange){
        PlayScene* scene = getPlayScene();
        scene->distance = closetDist;
        scene->Defense=target;
        scene->btnAttack->Visible=false;
        scene->SetDrawRadius(false);
        scene->ChooseAbilityDraw=true;
        scene->btnConfirm->Visible=true;
        scene->waitingForConfirm=true;
        gridPos.x=bestMove.x, gridPos.y=bestMove.y;
        Sprite::Move(bestMove.x*PlayScene::BlockSize+PlayScene::BlockSize/2, bestMove.y*PlayScene::BlockSize+PlayScene::BlockSize/2);
        calc=false;
        return true;
    }
    //cout<<Unit::damage;
    gridPos.x=bestMove.x, gridPos.y=bestMove.y;
    Sprite::Move(bestMove.x*PlayScene::BlockSize+PlayScene::BlockSize/2, bestMove.y*PlayScene::BlockSize+PlayScene::BlockSize/2);
    imgBase.Move(bestMove.x*PlayScene::BlockSize+PlayScene::BlockSize/2, bestMove.y*PlayScene::BlockSize+PlayScene::BlockSize/2);
    calc=false;
    return false;
}

void EnemyGunner1::chooseSkill(){
    PlayScene* Scene = getPlayScene();
    int maxEnergy = -1;
    int bestSkillIndex = -1;
    const auto& skills = GetSkills();
    for (int i = 0; i < (int)skills.size(); ++i) {
        if (Energy >= skills[i].energy && skills[i].energy > maxEnergy) {
            maxEnergy = skills[i].energy;
            bestSkillIndex = i;
        }
    }
    Scene->EnemyselectedSkillIndex = bestSkillIndex;
}

