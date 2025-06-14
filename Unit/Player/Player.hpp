#ifndef PLAYER_HPP
#define PLAYER_HPP
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

class Player : public Unit {
protected:
    int actionValue;
    int distance;
    float speed;
    int Maxhp;
    int hp;
    int attackRange;
    float damage;
    PlayScene *getPlayScene();
    // Reference: Design Patterns - Factory Method.
public:
    bool Enabled = true;
    Unit* target;
    Player(std::string base, std::string img, std::string fight_img, float x, float y, int hp, float speed, int distance, int damage, int energy,int attackRange, std::string Label);
    bool IsPlayer() const override { return true; }
    void Update(float deltaTime) override;
    void Draw() const override;
    bool CheckPlacement(int x, int y);
    bool TryMoveNearTarget(Unit* target);
    void Hit();

};
#endif   // PLAYER_HPP
