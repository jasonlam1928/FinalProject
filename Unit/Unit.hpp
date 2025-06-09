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
    Unit(float x, float y, std::string img, float speed, float hp, int distance);
    virtual ~Unit() = default;

    virtual bool IsPlayer() const = 0;
    virtual void Act() = 0;

    float Speed;
    float HP;
    float MAXHP;
    int distance;
    int attackRange;

    float ActionValue;  // 當前行動值
    static constexpr float MaxActionValue = 10000;

    bool calc;
    int drawStep = 0;
    float drawTimer = 0;
    float drawInterval = 0.02f; // 每層間隔動畫時間

    Engine::IntPoint gridPos;      // 原本 x0, y0
    Engine::IntPoint previewPos;   // 原本 Prex0, Prey0

    std::set<Engine::IntPoint> radius;
    std::map<Engine::IntPoint, int> radiusStep;
    std::map<Engine::IntPoint, bool> MoveValid;

    void Update(float deltaTime) override;
    bool UpdateActionValue(float deltaTime);
    void drawUI();
    void drawRadius(int cameraX, int cameraY);
    void UpdateRadiusAnimation(float deltaTime);
    bool CheckPlacement(int x, int y);
    void MovetoPreview();
    void CancelPreview();
};
