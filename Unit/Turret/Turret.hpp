#ifndef TURRET_HPP
#define TURRET_HPP
#include <allegro5/base.h>
#include <list>
#include <string>

#include "Engine/Sprite.hpp"
#include "Unit/Unit.hpp"

class Enemy;
class Bullet;
class PlayScene;
class Label;
class HitLabel;

class Turret : public Unit {
protected:
    int actionValue;
    int distance;
    float speed;
    int Maxhp;
    int hp;
    int attackRange;
    PlayScene *getPlayScene();
    // Reference: Design Patterns - Factory Method.
public:
    bool Enabled = true;

    Turret(std::string img, float x, float y, float hp, float speed, int distance);
    bool IsPlayer() const override { return true; }
    void Update(float deltaTime) override;
    void Draw() const override;
    virtual void OnExplode();
    void Hit(float damage, std::string Label);

};
#endif   // TURRET_HPP
