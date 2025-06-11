#include <allegro5/allegro_primitives.h>
#include <allegro5/color.h>
#include <cmath>
#include <random>
#include <string>
#include <vector>
#include <iomanip>
#include <sstream>

#include "Enemy.hpp"
#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Group.hpp"
#include "Engine/IScene.hpp"
#include "Engine/LOG.hpp"
#include "Scene/PlayScene.hpp"
#include "UI/Animation/DirtyEffect.hpp"
#include "UI/Animation/ExplosionEffect.hpp"
#include "UI/Component/HitLabel.hpp"

int count=0;
PlayScene *Enemy::getPlayScene() {
    return dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetActiveScene());
}

Enemy::Enemy(std::string img, float x, float y, float hp, float speed, int distance, float damage, std::string Label) : Unit(x, y,img, speed, hp, distance, damage, Label), distance(distance), speed(speed), hp(hp), damage(damage), Label(Label){
    Maxhp=hp;
    attackRange=2;
    img=img;
}

void Enemy::Hit(float damage, std::string Label) {
    
}

void Enemy::Update(float deltaTime) {

}
void Enemy::Draw() const {
    Sprite::Draw();
    
    
}
