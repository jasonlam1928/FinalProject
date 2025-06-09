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
    Enemy(std::string img, float x, float y, int hp, float speed, int distance);
    bool IsPlayer() const override { return false; }
    void Hit(float damage, std::string Label);
    void Update(float deltaTime) override;
    void Draw() const override;
    
};

