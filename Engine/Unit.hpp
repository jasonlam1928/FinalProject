// Unit.hpp
#pragma once
#include<set>
#include<queue>
#include<map>
using namespace std;
#include"Sprite.hpp"
class PlayScene; 
class Unit : public Engine::Sprite {
protected:
    PlayScene *getPlayScene();
public:
    Unit(float x, float y, std::string img, float speed, float hp, int distance);
    
    int x0, y0;
    int Prex0, Prey0;
    virtual ~Unit() = default;
    virtual bool IsPlayer() const = 0;
    virtual void Act() = 0;

    float Speed;
    float HP;
    float distance;

    float ActionValue;  // 當前行動值
    bool calc;
    set<pair<int,int>> radius;
    map<pair<int,int>,int> radiusStep;
    static constexpr float MaxActionValue = 10000;
    int drawStep = 0;
    float drawTimer = 0;
    float drawInterval = 0.02; // 每層間隔 0.1 秒，可調


    // 行動值減少邏輯
    bool UpdateActionValue(float deltaTime);
    void drawRadius(int cameraX, int cameraY);
    bool CheckPlacement(int x, int y);
    //檢查滑鼠位置係咪係radius入面
    void UpdateRadiusAnimation(float deltaTime);
    void MovetoPreview();
    void CancelPreview();
};
