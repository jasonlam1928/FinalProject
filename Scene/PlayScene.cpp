#include <algorithm>
#include <allegro5/allegro.h>
#include <ostream>
#include<iostream>
#include <cmath>
#include <fstream>
#include <functional>
#include <memory>
#include <queue>
#include <string>
#include <vector>
#include <list>
#include <queue>
#include <set>

#include "Enemy/Enemy.hpp"
#include "Enemy/Enemy1.hpp"
#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Group.hpp"
#include "Engine/LOG.hpp"
#include "Engine/Resources.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Component/ImageButton.hpp"
#include "PlayScene.hpp"
#include "Turret/Turret.hpp"
#include"Turret/Turret1.hpp"
#include "UI/Animation/DirtyEffect.hpp"

#include<queue>

using namespace std;

bool PlayScene::DebugMode = false;
const std::vector<Engine::Point> PlayScene::directions = { Engine::Point(-1, 0), Engine::Point(0, -1), Engine::Point(1, 0), Engine::Point(0, 1) };
const int PlayScene::MapWidth = 64, PlayScene::MapHeight = 64;
const int PlayScene::BlockSize = 96;
const pair<int,int> PlayScene::WindowSize={2200,1400};
const float PlayScene::DangerTime = 7.61;
const Engine::Point PlayScene::SpawnGridPoint = Engine::Point(-1, 0);
const Engine::Point PlayScene::EndGridPoint = Engine::Point(MapWidth, MapHeight - 1);
const std::vector<int> PlayScene::code = {
    ALLEGRO_KEY_UP, ALLEGRO_KEY_UP, ALLEGRO_KEY_DOWN, ALLEGRO_KEY_DOWN,
    ALLEGRO_KEY_LEFT, ALLEGRO_KEY_RIGHT, ALLEGRO_KEY_LEFT, ALLEGRO_KEY_RIGHT,
    ALLEGRO_KEY_B, ALLEGRO_KEY_A, ALLEGRO_KEYMOD_SHIFT, ALLEGRO_KEY_ENTER
};
Engine::Point PlayScene::GetClientSize() {
    return Engine::Point(MapWidth * BlockSize, MapHeight * BlockSize);
}
void PlayScene::Initialize() {
    mapState.clear();
    keyStrokes.clear();
    active = false;
    drawRadius=false;
    MoveTime=0;
    cameraToTarget=true;
    UnitMoving=false;
    AddNewObject(TileMapGroup = new Group());
    AddNewObject(GroundEffectGroup = new Group());
    AddNewObject(DebugIndicatorGroup = new Group());
    AddNewObject(UnitGroup = new Group());

    AddNewObject(BulletGroup = new Group());
    AddNewObject(EffectGroup = new Group());
    // Should support buttons.
    AddNewControlObject(UIGroup = new Group());
    ReadMap();
    ReadEnemyWave();
    ConstructUI();
    float screenW = Engine::GameEngine::GetInstance().GetScreenSize().x;
    float screenH = Engine::GameEngine::GetInstance().GetScreenSize().y;
    btnConfirm = new Engine::ImageButton("play/Idle.png", "play/Hover.png",
                                         200, 236, 120, 40);
    btnConfirm->SetOnClickCallback(std::bind(&PlayScene::ConfirmClick, this)) ;   
    AddNewControlObject(btnConfirm);                                 
    btnCancel  = new Engine::ImageButton("play/Idle.png",  "play/Hover.png",
                                         0, 0, 120, 40);
    btnCancel->SetOnClickCallback(std::bind(&PlayScene::CancelClick, this)) ;  
    AddNewControlObject(btnCancel);  
    btnConfirm->Visible = btnCancel->Visible = false;
    UIGroup->AddNewObject(btnConfirm);
    UIGroup->AddNewObject(btnCancel);
    // 设定回调

    // Add groups from bottom to top.
    
    imgTarget = new Engine::Image("play/target.png", 0, 0);
    imgTarget->Visible = false;
    preview = nullptr;
    UIGroup->AddNewObject(imgTarget);
    // Preload Lose Scene
    deathBGMInstance = Engine::Resources::GetInstance().GetSampleInstance("astronomia.ogg");
    Engine::Resources::GetInstance().GetBitmap("lose/benjamin-happy.png");
    // Start BGM.
    bgmId = AudioHelper::PlayBGM("play.ogg");

    while(!enemyWaveData.empty()){
        auto current = enemyWaveData.front();
        enemyWaveData.pop_front();
        Unit *unit;
        int gridX = current.second.first-1;
        int gridY = current.second.second-1;
        float posX = gridX * BlockSize + BlockSize / 2;
        float posY = gridY * BlockSize + BlockSize / 2;
        switch(current.first){
            case 1:
                UnitGroup->AddNewObject(unit = new Knight(posX, posY));
                break;
            case 2:
                UnitGroup->AddNewObject(unit = new Knight1(posX, posY));
                break;
        }
    }
    

}

void PlayScene::ConfirmClick(){
    waitingForConfirm=false;
    previewSelected = false;
    btnConfirm->Visible = btnCancel->Visible = false;
    Processing->MovetoPreview();
    Processing->CancelPreview();
    Processing=nullptr;
    auto it=Action.find(Processing);
    if(it!=Action.end()) Action.erase(it);
}
void PlayScene::CancelClick(){
    waitingForConfirm=false;
    previewSelected = false;
    btnConfirm->Visible = btnCancel->Visible = false;
    Processing->CancelPreview();
}
void PlayScene::Terminate() {
    AudioHelper::StopBGM(bgmId);
    AudioHelper::StopSample(deathBGMInstance);
    deathBGMInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
    IScene::Terminate();
}
void PlayScene::Update(float deltaTime) {
    // If we use deltaTime directly, then we might have Bullet-through-paper problem.
    // Reference: Bullet-Through-Paper=
    int i=1;
    if(FastMoving) i=2;
    if (movingUp) cameraY -= cameraSpeed * deltaTime *i;
    if (movingDown) cameraY += cameraSpeed * deltaTime*i;
    if (movingLeft) cameraX -= cameraSpeed * deltaTime*i;
    if (movingRight) cameraX += cameraSpeed * deltaTime*i;

    // clamp camera（唔俾移出地圖）float screenW = Engine::GameEngine::GetInstance().GetScreenSize().x;
    float screenW = Engine::GameEngine::GetInstance().GetScreenSize().x;
    float screenH = Engine::GameEngine::GetInstance().GetScreenSize().y;
    float maxCameraX = std::max(0.0f, static_cast<float>(MapWidth * BlockSize) - screenW);
    float maxCameraY = std::max(0.0f, static_cast<float>(MapHeight * BlockSize) - screenH);

    float dx = cameraTargetX - cameraX;
    float dy = cameraTargetY - cameraY;
    float dist = std::sqrt(dx * dx + dy * dy);
    float moveSpeed = 8000.0f; // 每秒移動幾多像素，可自行調節
    if(!cameraToTarget){
        if (dist > 100.0f) {
            float moveStep = moveSpeed * deltaTime;
            if (moveStep > dist) moveStep = dist;
            cameraX += dx / dist * moveStep;
            cameraY += dy / dist * moveStep;
        } else {
            cameraToTarget = true;
        }
    }
    cameraX = std::max(0.0f, std::min(cameraX, maxCameraX));
    cameraY = std::max(0.0f, std::min(cameraY, maxCameraY));
    
    for(auto obj:UnitGroup->GetObjects()){
        Unit* unit = dynamic_cast<Unit*>(obj);

        if(Action.count(unit)==0){
            Action.insert(unit);
        }
        else if(Action.count(unit)==1){
            Unit* prev = *Action.find(unit);
            unit->ActionValue+=prev->ActionValue;
            Action.insert(unit);
        }

    }
    for(auto unit:Action){
        if(unit->UpdateActionValue(deltaTime)){
            Managing.push(unit);
        }
    }
    
    
    if(!Managing.empty()&&Processing==nullptr){
        Processing=Managing.front();
        Managing.pop();
        drawRadius=true;
        Preview=Processing;
        Preview->drawStep=0;
        cameraTargetX=Processing->gridPos.x*BlockSize-WindowSize.first/2;
        cameraTargetY=Processing->gridPos.y*BlockSize-WindowSize.second/2;
        cameraToTarget=false;
    }

    if(Processing!=nullptr){
        if (!Processing->IsPlayer()) {
            MoveTime += deltaTime;
            if (MoveTime >= 2.0f) {
                Unit* p = Processing;
                p->Act(); // 如果你有定義玩家的 Act
                UnitMoving=true;
                MoveTime = 0;
            }
        }
    }
    if(Preview!=nullptr&&drawRadius){
        Preview->UpdateRadiusAnimation(deltaTime);
    }
    if(UnitMoving){
        MoveTime+=deltaTime;
        if(MoveTime>=1.0f){
            Processing=nullptr;
            Preview=nullptr;
            for(auto obj:UnitGroup->GetObjects()){
                Unit* unit=dynamic_cast<Unit*>(obj);
                unit->calc=false;
            }
            UnitMoving=false;
        }
    }
    if (waitingForConfirm && confirmUnit && btnCancel->Visible && btnConfirm->Visible) {
        // 1) 取单位世界坐标（格子中心）

        // 2) 转成屏幕坐标（UI 坐标系）
        float screenX = WindowSize.first-2000 - cameraX;
        float screenY = WindowSize.second-1000 - cameraY;
        //cout<<"X:"<<screenX<<" Y:"<<screenY<<endl;
        bx1=WindowSize.first-screenX;
        by1 = WindowSize.second-screenY;
        bx2 = bx1-200;
        btnConfirm->SetPosition(bx1, by1, cameraX, cameraY);
        btnCancel->SetPosition( bx2-200, by1, cameraX, cameraY);

        btnConfirm->Visible = btnCancel->Visible = true;
    }
    else {
        btnConfirm->Visible = btnCancel->Visible = false;
    }
}



void PlayScene::Draw() const {
    al_identity_transform(&Camera);
    al_translate_transform(&Camera, -cameraX, -cameraY);
    al_use_transform(&Camera);
    IScene::Draw();
    al_identity_transform(&Camera);
    al_use_transform(&Camera);
    if(Preview!=nullptr){
        if(drawRadius){
            Preview->drawRadius(cameraX, cameraY);
        }
    }
    //btnCancel->Draw();
    //btnConfirm->Draw();
}

void PlayScene::OnMouseDown(int button, int mx, int my) {

    if (previewSelected) {
        // 让底层 UIGroup 先处理（包含 Confirm/Cancel 按钮）
        IScene::OnMouseDown(button, mx, my);
        return;  // 不要再去做格子选中
    }
    float worldX = mx + cameraX;
    float worldY = my + cameraY;

    const int x = worldX / BlockSize;
    const int y = worldY / BlockSize;
    
    if (button == 1) { // 左鍵
        dragging = true;
        cameraToTarget = true;
        dragStartX = mx;
        dragStartY = my;
        cameraStartX = cameraX;
        cameraStartY = cameraY;
        if(Preview==Processing&&drawRadius){
            if(Processing->IsPlayer()){
                cout<<worldX<<" "<<bx2-240<<" "<<bx1+240<<" "<<worldY<<" "<<by1-80<<" "<<by1+80<<endl;
                if(!(worldX>=bx2-240&&worldX<=bx1+240&&worldY>=by1-80&&worldY<=by1+80)){
                    cout<<"out"<<endl;
                    if(Processing->CheckPlacement(x, y)){
                        waitingForConfirm=true;
                        confirmUnit   = Processing;
                        confirmTarget = Engine::Point(x, y);
                        btnConfirm->Visible=btnCancel->Visible=true;
                    }
                    else{
                        Processing->CancelPreview();
                        drawRadius=false;
                        btnConfirm->Visible = btnCancel->Visible = false;
                    } 
                }
                
            }
            else{
                drawRadius=false;
            } 
        }
        else{
            drawRadius=false;
            for(auto& obj:UnitGroup->GetObjects()){
                Unit* unit=dynamic_cast<Unit*>(obj);
                if(x==unit->gridPos.x&&y==unit->gridPos.y){
                    Preview=unit;
                    drawRadius=true;
                    Preview->drawStep=0;
                }
            }
        }
        
    }
    IScene::OnMouseDown(button, mx, my);
    
}
void PlayScene::OnMouseMove(int mx, int my) {
    IScene::OnMouseMove(mx, my);
    float worldX = mx + cameraX;
    float worldY = my + cameraY;
    const int x = worldX / BlockSize;
    const int y = worldY / BlockSize;
    if (dragging) {
        int dx = mx - dragStartX;
        int dy = my - dragStartY;
        cameraX = cameraStartX - dx;
        cameraY = cameraStartY - dy;

        // 限制邊界
        float screenW = Engine::GameEngine::GetInstance().GetScreenSize().x;
        float screenH = Engine::GameEngine::GetInstance().GetScreenSize().y;
        float maxCameraX = std::max(0.0f, static_cast<float>(MapWidth * BlockSize) - screenW);
        float maxCameraY = std::max(0.0f, static_cast<float>(MapHeight * BlockSize) - screenH);
        cameraX = std::max(0.0f, std::min(cameraX, maxCameraX));
        cameraY = std::max(0.0f, std::min(cameraY, maxCameraY));
    }

}
void PlayScene::OnMouseUp(int button, int mx, int my) {
    IScene::OnMouseUp(button, mx, my);
    float worldX = mx + cameraX;
    float worldY = my + cameraY;
    const int x = worldX / BlockSize;
    const int y = worldY / BlockSize;
    if (button == 1) {
        dragging = false;
    }
    
}

void PlayScene::OnKeyDown(int keyCode) {
    IScene::OnKeyDown(keyCode);
    if (keyCode == ALLEGRO_KEY_W) movingUp = true;
    if (keyCode == ALLEGRO_KEY_S) movingDown = true;
    if (keyCode == ALLEGRO_KEY_A) movingLeft = true;
    if (keyCode == ALLEGRO_KEY_D) movingRight = true;
    if(keyCode==ALLEGRO_KEY_LSHIFT||keyCode==ALLEGRO_KEY_RSHIFT) FastMoving = true;
    if (keyCode == ALLEGRO_KEY_TAB) {
        DebugMode = !DebugMode;
    }
    else if(keyCode == ALLEGRO_KEY_F1){
        Scores();
        Engine::GameEngine::GetInstance().ChangeScene("win");
    } else {
        //std::cout << "Key pressed: " << keyCode << std::endl;
        if(keyCode==ALLEGRO_KEY_LSHIFT||keyCode==ALLEGRO_KEY_RSHIFT){
            keyStrokes.push_back(ALLEGRO_KEYMOD_SHIFT);
        }
        else{
            keyStrokes.push_back(keyCode);
        }
        if (keyStrokes.size() > code.size())
            keyStrokes.pop_front();
    }

    if(keyStrokes.size()==code.size()){
        bool match = true;
        auto it = keyStrokes.begin();
        for(int i=0;i<code.size();i++, it++){
            if(*it!=code[i]){
                match = false;
                break;
            }     
        }
        if(match){
            EarnMoney(10000);
            std::cout << "Cheat code activated!" << std::endl;
            keyStrokes.clear();
        }
        else{
            std::cout << "Cheat code not matched." << std::endl;
        }
    }

    if (keyCode >= ALLEGRO_KEY_0 && keyCode <= ALLEGRO_KEY_9) {
        // Hotkey for Speed up.
        SpeedMult = keyCode - ALLEGRO_KEY_0;
    }
}

void PlayScene::OnKeyUp(int keyCode){
    IScene::OnKeyUp(keyCode);
    if (keyCode == ALLEGRO_KEY_W) movingUp = false;
    if (keyCode == ALLEGRO_KEY_S) movingDown = false;
    if (keyCode == ALLEGRO_KEY_A) movingLeft = false;
    if (keyCode == ALLEGRO_KEY_D) movingRight = false;
    if(keyCode==ALLEGRO_KEY_LSHIFT||keyCode==ALLEGRO_KEY_RSHIFT) FastMoving = false;
}

void PlayScene::Hit() {
    lives--;
    if (lives <= 0) {
        Scores();
        Engine::GameEngine::GetInstance().ChangeScene("lose");
    }
}
int PlayScene::GetMoney() const {
    return money;
}
void PlayScene::EarnMoney(int money) {

}
void PlayScene::ReadMap() {
    std::string filename = std::string("Resource/map") + std::to_string(MapId) + ".txt";
    // Read map file.
    char c;
    std::vector<bool> mapData;
    std::ifstream fin(filename);
    while (fin >> c) {
        switch (c) {
            case '0': mapData.push_back(false); break;
            case '1': mapData.push_back(true); break;
            case '\n':
            case '\r':
            default: throw std::ios_base::failure("Map data is corrupted.");
        }
    }
    fin.close();

    mapState = std::vector<std::vector<TileType>>(MapHeight, std::vector<TileType>(MapWidth));
    for (int i = 0; i < MapHeight; i++) {
        for (int j = 0; j < MapWidth; j++) {
            const int num = mapData[i * MapWidth + j];
            mapState[i][j] = num ? TILE_FLOOR : TILE_DIRT;
            if (num)
                TileMapGroup->AddNewObject(new Engine::Image("play/floor.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
            else
                TileMapGroup->AddNewObject(new Engine::Image("play/dirt.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
        }
    }
}
void PlayScene::ReadEnemyWave() {
    std::string filename = std::string("Resource/enemy") + std::to_string(MapId) + ".txt";
    // Read enemy file.
    float type, x, y;
    enemyWaveData.clear();
    std::ifstream fin(filename);
    while (fin >> type && fin >> x && fin >> y) {
        enemyWaveData.emplace_back(type, std::make_pair(x, y));

    }
    fin.close();
}
void PlayScene::ConstructUI() {

}
void PlayScene::UIBtnClicked(int id) {
    
}

bool PlayScene::CheckSpaceValid(int x, int y) {
    return true;
}
std::vector<std::vector<int>> PlayScene::CalculateBFSDistance() {
    // Reverse BFS to find path.
    std::vector<std::vector<int>> map(MapHeight, std::vector<int>(std::vector<int>(MapWidth, -1)));
    std::queue<Engine::Point> que;
    // Push end point.
    // BFS from end point.
    if (mapState[MapHeight - 1][MapWidth - 1] != TILE_DIRT)
        return map;
    que.push(Engine::Point(MapWidth - 1, MapHeight - 1));
    map[MapHeight - 1][MapWidth - 1] = 0;
    while (!que.empty()) {
        Engine::Point p = que.front();
        que.pop();

        for (auto dir : directions) {
            int nx = p.x + dir.x;
            int ny = p.y + dir.y;

            // Check boundaries
            if (nx >= 0 && nx < MapWidth && ny >= 0 && ny < MapHeight) {
                // Only proceed if it's TILE_DIRT and not visited
                if (mapState[ny][nx] == TILE_DIRT && map[ny][nx] == -1) {
                    map[ny][nx] = map[p.y][p.x] + 1;
                    que.push(Engine::Point(nx, ny));
                }
            }
        }
        // TODO PROJECT-1 (1/1): Implement a BFS starting from the most right-bottom block in the map.
        //               For each step you should assign the corresponding distance to the most right-bottom block.
        //               mapState[y][x] is TILE_DIRT if it is empty.
    }
    return map;
}
int score=0;
void PlayScene::Scores(){
    
    if(lives!=0){
        score+=500;
    }
}

int getScore(){
    return score;
}

void PlayScene::ClearPlace(int x, int y){
    x /= BlockSize;
    y/=BlockSize;
    mapState[y][x] = TILE_DIRT;
}