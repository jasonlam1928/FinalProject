#include <string>
#include<iostream>
#include "EnemyKnight1.hpp"
#include"Scene/PlayScene.hpp"
#include"Engine/IntPoint.hpp"

EnemyKnight1::EnemyKnight1(int x, int y) : Enemy("play/EnemyBase.png","play/ShieldIcon.png","play/KnightIdle.png", x, y, 100, 10, 5, 2, 100,1, "Knight_01") {
    AddSkill({"Blade", 1, 5, 100, 0.2});
    AddSkill({"Defense", 1, 0, 0, 0});
    attackRange=1;
}

//SKILL:Name, Energy, Power, CritChance
bool EnemyKnight1::Act(){
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
                if(dist<closetDist){
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
        scene->Defense=target;
        scene->btnAttack->Visible=false;
        scene->SetDrawRadius(false);
        scene->ChooseAbilityDraw=true;
        scene->btnConfirm->Visible=true;
        scene->waitingForConfirm=true;
        scene->distance = closetDist;
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

void EnemyKnight1::chooseSkill(){
    PlayScene* Scene = getPlayScene();
    int maxEnergy = -1;
    int bestSkillIndex = -1;
    const auto& skills = GetSkills();
    for (int i = 0; i < (int)skills.size(); ++i) {
        if (Energy >= skills[i].energy && skills[i].energy > maxEnergy && Scene->distance<=skills[i].range) {
            maxEnergy = skills[i].energy;
            bestSkillIndex = i;
        }
    }
    Scene->EnemyselectedSkillIndex = bestSkillIndex;
    cout<<bestSkillIndex<<endl;
}

