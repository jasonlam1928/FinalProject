// Unit.hpp
#pragma once
#include <set>
#include <queue>
#include <map>
#include <string>
#include "Engine/Sprite.hpp"
#include "Engine/IntPoint.hpp"

class PlayScene;

class Unit : public Engine::Sprite {
protected:
    PlayScene* getPlayScene();

public:
    Unit(float x, float y, std::string img, float speed, float hp, int distance, float damage, std::string Label);
    virtual ~Unit() = default;

    virtual bool IsPlayer() const = 0;
    virtual void Act() = 0;

    float Speed;
    float HP;
    float MAXHP;
    int distance;
    int attackRange;
    float damage;
    std::string Label;

    float ActionValue;  // 當前行動值
    inline static float MaxActionValue = 20.0f;

    bool calc;
    int drawStep = 0;
    float drawTimer = 0;
    float drawInterval = 0.02f; // 每層間隔動畫時間

    Engine::IntPoint gridPos;  
    Engine::IntPoint previewPos;   
    std::set<Engine::IntPoint> radius;
    std::map<Engine::IntPoint, int> radiusStep;
    std::map<Engine::IntPoint, bool> MoveValid;

    void Update(float deltaTime) override;
    bool UpdateActionValue(float deltaTime);
    void DrawUI();
    void drawRadius(int cameraX, int cameraY);
    void UpdateRadiusAnimation(float deltaTime);
    void UnitHit(float UnitDamage);
    void MovetoPreview();
    void CancelPreview();
    bool TryMoveNearTarget(Unit* target);
};
