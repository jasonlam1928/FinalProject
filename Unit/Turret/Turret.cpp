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
Turret::Turret(std::string img, float x, float y, float hp, float speed, int distance) : Unit(x, y, img, speed, hp, distance){

}

void Turret::Update(float deltaTime) {
    
}
void Turret::OnExplode() {
    
}

void Turret::Hit(float damage, std::string Label) {
    hp -= damage;
    
}
void Turret::Draw() const {
    Sprite::Draw();
}



