#pragma once
#include <list>
#include <string>
#include <vector>

#include "Engine/Point.hpp"
#include "Engine/Sprite.hpp"
#include"Unit/Unit.hpp"

class Bullet;
class PlayScene;
class Player;

class Enemy : public Unit {
protected:
    PlayScene *getPlayScene() const;
    std::list<Enemy *>::iterator lockedEnemyIterator;

public:
    int distance;
    float speed;
    int Maxhp;
    int hp;
    int attackRange;
    int damage;
    Unit* target;
    std::string img;
    std::string Label;
    Enemy(std::string base, std::string img, std::string fight_img, float x, float y, int hp, float speed, int distance, int damage, int energy,int attackRange, std::string Label);
    bool IsPlayer() const override { return false; }
    void Hit(float damage, std::string Label);
    void Update(float deltaTime) override;
    void Draw() const override;
    
};

