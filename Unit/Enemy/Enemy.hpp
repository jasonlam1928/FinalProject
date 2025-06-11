#pragma once
#include <list>
#include <string>
#include <vector>

#include "Engine/Point.hpp"
#include "Engine/Sprite.hpp"
#include"Unit/Unit.hpp"

class Bullet;
class PlayScene;
class Turret;


class Enemy : public Unit {
protected:
    PlayScene *getPlayScene();
    std::list<Enemy *>::iterator lockedEnemyIterator;

public:
    int distance;
    float speed;
    float Maxhp;
    float hp;
    int attackRange;
    float damage;
    Unit* target;
    std::string img;
    std::string Label;
    Enemy(std::string img, float x, float y, float hp, float speed, int distance, float damage, std::string Label);
    bool IsPlayer() const override { return false; }
    void Hit(float damage, std::string Label);
    void Update(float deltaTime) override;
    void Draw() const override;
    Enemy* Clone();
    
};

