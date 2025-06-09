#ifndef ENEMY_HPP
#define ENEMY_HPP
#include <list>
#include <string>
#include <vector>

#include "Engine/Point.hpp"
#include "Engine/Sprite.hpp"
#include"Engine/Unit.hpp"

class Bullet;
class PlayScene;
class Turret;


class Enemy : public Unit {
protected:
    PlayScene *getPlayScene();
    std::list<Enemy *>::iterator lockedEnemyIterator;

public:
    int actionValue;
    int distance;
    float speed;
    int Maxhp;
    int hp;
    int attackRange;
    Enemy(std::string img, float x, float y, float hp, float speed, int distance);
    bool IsPlayer() const override { return false; }
    void Hit(float damage, std::string Label);
    void Update(float deltaTime) override;
    void Draw() const override;
    
};

#endif   // ENEMY_HPP
