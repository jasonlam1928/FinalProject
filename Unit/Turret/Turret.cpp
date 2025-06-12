#include <allegro5/allegro_primitives.h>
#include <allegro5/color.h>
#include <cmath>
#include <utility>
#include <random>
#include <string>
#include <vector>
#include <iomanip>
#include <sstream>
#include<iostream>
#include "UI/Component/Label.hpp"
#include "UI/Component/HitLabel.hpp"
#include "Unit/Enemy/Enemy.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/AudioHelper.hpp"
#include "Engine/Group.hpp"
#include "Engine/IObject.hpp"
#include "Engine/IScene.hpp"
#include "Engine/Point.hpp"
#include "Engine/IntPoint.hpp"
#include "Scene/PlayScene.hpp"
#include "UI/Animation/ExplosionEffect.hpp"
#include "UI/Animation/DirtyEffect.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Slider.hpp"
#include "Turret.hpp"
#include "Scene/PlayScene.hpp"



PlayScene *Turret::getPlayScene() {
    return dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetActiveScene());
}
Turret::Turret(std::string img, float x, float y, int hp, float speed, int distance, int damage, int energy, std::string Label)
    : Unit(x, y, img, speed, hp, distance, damage, energy, Label), distance(distance), speed(speed), hp(hp), damage(damage) {
    Maxhp=hp;
    attackRange=2;
}

void Turret::Update(float deltaTime) {
    
}

// Turret.cpp
bool Turret::TryMoveNearTarget(Unit* target) {
    PlayScene* scene = getPlayScene();
    Engine::IntPoint start = target->gridPos;               // 自己目前的位置     // 敵人位置

    std::queue<std::pair<Engine::IntPoint,int>> q;
    std::set<Engine::IntPoint> visited;
    q.push({start, 0});
    visited.insert(start);
    int dx[4] = {0,0,1,-1};
    int dy[4] = {1,-1,0,0};
    Engine::IntPoint bestnxt;
    int bestUnitStep=INT_MAX;
    while (!q.empty()) {
        auto [cur, step] = q.front(); q.pop();  
        if (step >= 0) {
            for(int i=0;i<4;i++){
                Engine::IntPoint nxt=cur;
                bool valid=true;
                 nxt.x+=dx[i];
                 nxt.y+=dy[i];
                 
                 if(!MoveValid[nxt]) continue;
                 
                 if(nxt.x<0||nxt.x>=PlayScene::MapWidth-1||nxt.y<0||nxt.y>=PlayScene::MapHeight) continue;
                 
                 if(visited.count(nxt)) continue;
                 for(auto obj:getPlayScene()->UnitGroup->GetObjects()){
                    Unit* unit = dynamic_cast<Unit*>(obj);
                    if(nxt==unit->gridPos){
                        valid=false;
                        break;
                    }
                 }
                 
                 if(step==attackRange-1){
                    if(radiusStep[nxt]<bestUnitStep){
                        bestnxt=nxt;
                        bestUnitStep=radiusStep[nxt];
                        valid=false;
                    }
                 }
                 if(step>attackRange-1&&bestUnitStep!=INT_MAX)continue;
                 if(nxt==this->previewPos) return true;
                 if(!valid) continue;
                 q.push({nxt, step+1});
                 visited.insert(nxt);
                 
            }
           
            
        }
    }
    //cout<<bestnxt.x<<" "<<bestnxt.y<<endl;
    Unit::previewPos = bestnxt;
    Sprite::Move(bestnxt.x*PlayScene::BlockSize+PlayScene::BlockSize/2, bestnxt.y*PlayScene::BlockSize+PlayScene::BlockSize/2);
    return true;
}

bool Turret::CheckPlacement(int x, int y) {
    Engine::IntPoint p(x, y);
    PlayScene* scene = getPlayScene();

    // 檢查點擊到敵人
    for (auto& obj : scene->UnitGroup->GetObjects()) {
        Unit* unit = dynamic_cast<Unit*>(obj);
        if (unit && unit->gridPos == p && !unit->IsPlayer()) {
            if (radius.count(p)) {
                target=unit;
                if (TryMoveNearTarget(unit)){
                    Unit* turret = this;
                    getPlayScene()->Defense=unit;
                    getPlayScene()->attackUIDraw=true;
                    return true;
                }
            }
        }
    }
    // 否則做一般移動
    if (MoveValid[p]) {
        getPlayScene()->attackUIDraw=false;
        Unit::previewPos = p;
        Sprite::Move(
            p.x * PlayScene::BlockSize + PlayScene::BlockSize/2,
            p.y * PlayScene::BlockSize + PlayScene::BlockSize/2
        );
        return true;
    }

    return false;
}

void Turret::Hit() {
    
}
void Turret::Draw() const {
    Sprite::Draw();
}



