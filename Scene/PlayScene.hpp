#ifndef PLAYSCENE_HPP
#define PLAYSCENE_HPP
#include <allegro5/allegro_audio.h>
#include <list>
#include <memory>
#include <utility>
#include <vector>
#include<set>
#include<queue>
using namespace std;

#include "Engine/IScene.hpp"
#include "Engine/Point.hpp"
#include "Unit/Unit.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"

class Turret;
namespace Engine {
    class Group;
    class Image;
    class Label;
    class Sprite;
}   // namespace Engine

extern int score;
class PlayScene final : public Engine::IScene {
private:
    enum TileType {
        TILE_DIRT,
        TILE_FLOOR,
        TILE_OCCUPIED,
    };
    ALLEGRO_SAMPLE_ID bgmId;
    std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> deathBGMInstance;

protected:
    int lives;
    int money;
    int SpeedMult;
    bool isShovelMode;
    bool movingUp = false;
    bool movingDown = false;
    bool movingLeft = false;
    bool movingRight = false;
    bool FastMoving=false;
    bool dragging = false;
    int dragStartX = 0, dragStartY = 0;
    float cameraStartX = 0, cameraStartY = 0;
    int cameraTargetX = cameraX;
    int cameraTargetY = cameraY;
    bool drawRadius;
    int bx1;//buttonX
    int by1;//buttonY
    int bx2;
    int by2;
    int bx3;
    int by3;

    void ConfirmClick();
    void CancelClick();
    void AttackClick();

public:
    static bool DebugMode;
    static const std::vector<Engine::Point> directions;
    static const int MapWidth, MapHeight;
    static const int BlockSize;
    static const float DangerTime;
    static const pair<int,int> WindowSize;
    static const Engine::Point SpawnGridPoint;
    static const Engine::Point EndGridPoint;
    static const std::vector<int> code;
    int MapId;
    float ticks;
    float deathCountDown;
    // Map tiles.
    Group *TileMapGroup;
    Group *GroundEffectGroup;
    Group *DebugIndicatorGroup;
    Group *BulletGroup;
    Group *UnitGroup;
    Group *EffectGroup;
    Group *UIGroup;
    Unit *Processing;
    Unit *Preview;
    Engine::Image *imgTarget;
    Turret *preview;
    std::vector<std::vector<TileType>> mapState;
    std::vector<std::vector<int>> mapDistance;
    std::list<std::pair<int, std::pair<int, int>>> enemyWaveData;
    std::list<int> keyStrokes;
    static Engine::Point GetClientSize();
    explicit PlayScene() = default;
    void Initialize() override;
    void Terminate() override;
    void Update(float deltaTime) override;
    void Draw() const override;
    void AttackUI()const;
    void OnMouseDown(int button, int mx, int my) override;
    void OnMouseMove(int mx, int my) override;
    void OnMouseUp(int button, int mx, int my) override;
    void OnKeyDown(int keyCode) override;
    void OnKeyUp(int keyCode) override;
    void Hit();
    int GetMoney() const;
    void EarnMoney(int money);
    void ReadMap();
    void ReadEnemyWave();
    void ConstructUI();
    void UIBtnClicked(int id);
    bool CheckSpaceValid(int x, int y);
    void Scores();
    void ClearPlace(int x, int y);
    std::vector<std::vector<int>> CalculateBFSDistance();
    pair<int,int> GetCamera();
    // void ModifyReadMapTiles();

    mutable ALLEGRO_TRANSFORM Camera;
    vector<Unit*> Action;
    queue<Unit*> Managing;
    bool changeCamera;
    float cameraX = 0;
    float cameraY = 0;
    float cameraSpeed = 600; // pixels per second
    float cameraTargetSpeed=0.5;
    bool cameraToTarget;
    bool UnitMoving;
    bool waitingForConfirm = false;
    bool previewSelected = false;
    Engine::Point confirmTarget;  
    Engine::ImageButton *btnConfirm, *btnCancel, *btnAttack;
    Unit *confirmUnit = nullptr;

    Unit* Defense;
    bool attackUIDraw=false;
    float AttackUIVisibleTime = 0;
    bool attackUIActive = false;

    int actionValue = 100;
    bool active;
    float MoveTime;
};

int getScore();

#endif   // PLAYSCENE_HPP
