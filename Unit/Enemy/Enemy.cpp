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
PlayScene *Enemy::getPlayScene() const {
    return dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetActiveScene());
}

Enemy::Enemy(std::string base, std::string img, std::string fight_img, float x, float y, int hp, float speed, int distance, int damage, int energy, int attackRange,std::string Label)
    : Unit(x, y, base, img, fight_img, speed, hp, distance, damage, energy,attackRange, Label), distance(distance), speed(speed), hp(hp), damage(damage), Label(Label) {
    Maxhp=hp;
    img=img;
}

void Enemy::Hit(float damage, std::string Label) {
    
}

void Enemy::Update(float deltaTime) {
    PlayScene* scene = getPlayScene();
    imgBase.Move(Sprite::Position.x, Sprite::Position.y);
    Select.Move(Sprite::Position.x, Sprite::Position.y);
    
}
void Enemy::Draw() const {
    imgBase.Draw();
    Sprite::Draw();
    if(getPlayScene()->Processing==this){
        Select.Draw();
    }
}

