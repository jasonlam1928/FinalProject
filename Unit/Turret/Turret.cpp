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
Turret::Turret(std::string img, float x, float y, float hp, float speed, int distance, float damage) : Unit(x, y, img, speed, hp, distance, damage), distance(distance), speed(speed), hp(hp), damage(damage) {
    Maxhp=hp;
}

void Turret::Update(float deltaTime) {
    
}

bool Turret::CheckPlacement(int x, int y) {
    Engine::IntPoint p(x, y);

    if (!MoveValid[p]){
        for(auto obj:getPlayScene()->UnitGroup->GetObjects()){
            Unit* unit=dynamic_cast<Unit*>(obj);
            if(unit->gridPos==p&&!unit->IsPlayer()){
                unit->UnitHit(damage);
                return false;
            }
        }
    }

    for (auto& r : radius) {
        
        if (r == p) {
            previewPos = p;
            Sprite::Move(p.x * PlayScene::BlockSize + PlayScene::BlockSize/2, p.y * PlayScene::BlockSize + PlayScene::BlockSize/2);
            return true;
        }
    }
    return false;
}

void Turret::Hit() {
    
}
void Turret::Draw() const {
    Sprite::Draw();
}



