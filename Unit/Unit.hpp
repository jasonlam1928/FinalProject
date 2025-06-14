// Unit.hpp
#pragma once
#include <set>
#include <queue>
#include <map>
#include <string>
#include <vector>
#include "Engine/Sprite.hpp"
#include "Engine/IntPoint.hpp"

class PlayScene;

struct Skill {
    std::string name;
    int range = 0; // 技能範圍
    int energy = 0;
    int power = 0; // 技能傷害或效果強度
    float critRate = 0.0f; // 暴擊機率
    // 可擴充更多屬性，如 icon、效果 function pointer 等
};

class Unit : public Engine::Sprite {
protected:
    PlayScene* getPlayScene();
    std::vector<Skill> skills; // 最多4個技能

public:
    Unit(float x, float y,std::string base, std::string img, std::string fight_img, float speed, int hp, int distance, int damage, int energy,int attackRange, std::string Label);
    virtual ~Unit() = default;

    virtual bool IsPlayer() const = 0;
    virtual bool Act() = 0;
    virtual void chooseSkill()=0;
    virtual  Unit* Clone()=0;

    float Speed;
    int HP;
    int MAXHP;
    int distance;
    int attackRange;
    int damage;
    std::string Label;
    std::string img;
    std::string fight_img;
    Sprite imgBase;
    int MaxEnergy;
    int Energy;

    float ActionValue;  // 當前行動值
    inline static float MaxActionValue = 50.0f;

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
    void AttackAbilityUI();
    void drawRadius(int cameraX, int cameraY);
    void UpdateRadiusAnimation(float deltaTime);
    void UnitHit(float UnitDamage);
    void MovetoPreview();
    void CancelPreview();
    bool TryMoveNearTarget(Unit* target);
    void Reset();

    void AddSkill(const Skill& skill);
    const std::vector<Skill>& GetSkills() const;
};
