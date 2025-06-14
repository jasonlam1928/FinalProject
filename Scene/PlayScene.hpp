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

class Player;
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
    int cameraTargetX = 0;
    int cameraTargetY=0;
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
    static int MapWidth, MapHeight;
    static const int BlockSize;
    static const pair<int,int> WindowSize;
    std::vector<pair<bool, bool>> mapData;
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
    Player *preview;
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
    void ChooseAbilityUI()const;
    void OnMouseDown(int button, int mx, int my) override;
    void OnMouseMove(int mx, int my) override;
    void OnMouseUp(int button, int mx, int my) override;
    void OnKeyDown(int keyCode) override;
    void OnKeyUp(int keyCode) override;
    void ReadMap();
    void ReadEnemyWave();
    void ConstructUI();
    void DrawActionValue(const std::vector<Unit*>& sortedAction, int y, float iconSize, float cellW, float cellH, int showCount) const;
    void AttackSystem();
    void CalcAttackValue();

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
    Engine::ImageButton *btnConfirm, *btnAbilityCancel, *btnAttack;
    Unit *confirmUnit = nullptr;
    int KnightCount, GunnerCount, kMaxUnits;

    Unit* Defense;
    bool attackUIDraw=false;
    bool ChooseAbilityDraw=false;
    float AttackUIVisibleTime = 0;
    bool attackUIActive = false;

    float actionValue;
    bool active;
    float MoveTime;

    mutable std::vector<std::pair<int, Engine::Point>> actionCellRects;
    mutable int PlayerselectedSkillIndex = -1; // -1 代表未選
    mutable int EnemyselectedSkillIndex = -1; // -1 代表未選
    void SetDrawRadius(bool value);
    bool isUnitInGroup(Unit* unit) const;
    void RemoveUnit(Unit* unit);
    int lastAttackDamage = 0;
    int lastCounterDamage = 0;

    ALLEGRO_FONT* attackUIFont = nullptr;
    ALLEGRO_FONT* bigFont = nullptr;
    ALLEGRO_FONT* font20 = nullptr;
    ALLEGRO_FONT* font22 = nullptr;
    ALLEGRO_FONT* font28 = nullptr;
    ALLEGRO_FONT* font32 = nullptr;
    ALLEGRO_FONT* font48 = nullptr;

    int mapWidth = 64;
    int mapHeight = 64;

    std::vector<Unit*> availableUnits; // 玩家可用但未放置的 unit
    Unit* draggingUnit = nullptr;      // 當前拖曳中的 unit
    float draggingOffsetX = 0, draggingOffsetY = 0;
    bool isPlacingUnits = true;        // 佈陣階段
    Engine::ImageButton* btnStartGame = nullptr; // 開始遊戲按鈕

    // 玩家已放置的 unit 記錄
    std::vector<Unit*> placedUnits;

    // 玩家可用角色（合併同類型，指標+數量）
    struct UnitSlot {
        Unit* proto;
        int count;
    };
    std::vector<UnitSlot> availableUnitSlots;

    int draggingUnitSlotIndex = -1; // 拖曳中的 unit slot 索引，-1 代表沒有拖曳

    Engine::ImageButton* btnShovel = nullptr; // 鏟子按鈕

    
};
int getScore();


#endif   // PLAYSCENE_HPP
