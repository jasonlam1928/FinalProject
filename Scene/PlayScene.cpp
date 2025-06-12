#include <algorithm>
#include <allegro5/allegro_primitives.h>
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

#include "Unit/Enemy/Enemy.hpp"
#include "Unit/Enemy/Enemy1.hpp"
#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Group.hpp"
#include "Engine/LOG.hpp"
#include "Engine/Resources.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Component/ImageButton.hpp"
#include "PlayScene.hpp"
#include "Unit/Turret/Turret.hpp"
#include"Unit/Turret/Turret1.hpp"
#include "UI/Animation/DirtyEffect.hpp"

#include<queue>

using namespace std;

bool PlayScene::DebugMode = false;
const std::vector<Engine::Point> PlayScene::directions = { Engine::Point(-1, 0), Engine::Point(0, -1), Engine::Point(1, 0), Engine::Point(0, 1) };
const int PlayScene::MapWidth = 64, PlayScene::MapHeight = 64;
const int PlayScene::BlockSize = 64;
const pair<int,int> PlayScene::WindowSize={1600,832};
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
    AddNewObject(EffectGroup = new Group());
    // Should support buttons.
    AddNewControlObject(UIGroup = new Group());
    ReadMap();
    ReadEnemyWave();
    ConstructUI();
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
    
    for(auto obj:UnitGroup->GetObjects()){
        Unit* unit = dynamic_cast<Unit*>(obj);
        Action.push_back(unit);
    }

}

void PlayScene::AttackSystem(){
    if (!Processing || !isUnitInGroup(Processing)) return;
    if (!Defense || !isUnitInGroup(Defense)) return;
    lastAttackDamage = 0;
    lastCounterDamage = 0;
    if(Processing->IsPlayer()){
        Processing->MovetoPreview();
        Processing->CancelPreview();
        if(PlayerselectedSkillIndex<0||PlayerselectedSkillIndex>=Processing->GetSkills().size()) return;
        int damage = Processing->GetSkills()[PlayerselectedSkillIndex].power;
        Processing->Energy-=Processing->GetSkills()[PlayerselectedSkillIndex].energy;
        Defense->UnitHit(damage);
        lastAttackDamage = damage;
        // 防呆：UnitHit 可能導致 Defense 被移除
        if (!Defense || !isUnitInGroup(Defense) || Defense->HP<=0) return;
        Defense->chooseSkill();
        int EnemyDamage = Defense->GetSkills()[EnemyselectedSkillIndex].power;
        Defense->Energy-=Defense->GetSkills()[EnemyselectedSkillIndex].energy;
        Processing->UnitHit(EnemyDamage);
        lastCounterDamage = EnemyDamage;
        // 防呆：UnitHit 可能導致 Processing 被移除
        if (!Processing || !isUnitInGroup(Processing)) return;
    }
    else{
        Processing->chooseSkill();
        int EnemyDamage = Processing->GetSkills()[EnemyselectedSkillIndex].power;
        Processing->Energy-=Processing->GetSkills()[EnemyselectedSkillIndex].energy;
        Defense->UnitHit(EnemyDamage);
        lastAttackDamage = EnemyDamage;
        // 防呆：UnitHit 可能導致 Defense 被移除
        if (!Defense || !isUnitInGroup(Defense) || Defense->HP<=0) return;
        int damage = Defense->GetSkills()[PlayerselectedSkillIndex].power;
        Defense->Energy-=Defense->GetSkills()[PlayerselectedSkillIndex].energy;
        Processing->UnitHit(damage);
        lastCounterDamage = damage;
        // 防呆：UnitHit 可能導致 Processing 被移除
        if (!Processing || !isUnitInGroup(Processing)) return;
    }
}

bool PlayScene::isUnitInGroup(Unit* unit) const {
    if (!unit) return false;
    for (auto obj : UnitGroup->GetObjects()) {
        if (obj == unit) return true;
    }
    return false;
}


void PlayScene::CalcAttackValue(){

}


void PlayScene::ConfirmClick(){
    if(btnConfirm->Visible==false) return;
    
    if(ChooseAbilityDraw){
        AttackSystem();
        btnConfirm->Visible = btnAbilityCancel->Visible = btnAttack->Visible=false;
        Processing->Reset();
        attackUIActive=true;
        AttackUIVisibleTime=1;
        return;
    }
    waitingForConfirm=false;
    previewSelected = false;
    btnConfirm->Visible = btnAbilityCancel->Visible = btnAttack->Visible=false;
    Processing->MovetoPreview();
    Processing->CancelPreview();
    Processing->Reset();
    Processing=nullptr;
}
void PlayScene::CancelClick(){
    if(btnAbilityCancel->Visible==false || !Processing->IsPlayer()) return;
    btnAbilityCancel->Visible=false;
    ChooseAbilityDraw=false;
    Processing->CancelPreview();
    drawRadius=true;
}
void PlayScene::AttackClick(){
    if(btnAttack->Visible==false) return;
    btnAttack->Visible=false;
    drawRadius=false;
    ChooseAbilityDraw=true;
}

void PlayScene::Terminate() {
    AudioHelper::StopBGM(bgmId);
    AudioHelper::StopSample(deathBGMInstance);
    deathBGMInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
    if (attackUIFont) {
        al_destroy_font(attackUIFont);
        attackUIFont = nullptr;
    }
    if (font20) { al_destroy_font(font20); font20 = nullptr; }
    if (font22) { al_destroy_font(font22); font22 = nullptr; }
    if (font28) { al_destroy_font(font28); font28 = nullptr; }
    if (font32) { al_destroy_font(font32); font32 = nullptr; }
    if (font48) { al_destroy_font(font48); font48 = nullptr; }
    IScene::Terminate();
}
void PlayScene::Update(float deltaTime) {
    // If we use deltaTime directly, then we might have Bullet-through-paper problem.
    // Reference: Bullet-Through-Paper=
    
    int i=1;
    if(FastMoving) i=2;
    if(!ChooseAbilityDraw){
        if (movingUp) cameraY -= cameraSpeed * deltaTime *i;
        if (movingDown) cameraY += cameraSpeed * deltaTime*i;
        if (movingLeft) cameraX -= cameraSpeed * deltaTime*i;
        if (movingRight) cameraX += cameraSpeed * deltaTime*i;
    }
    else attackUIDraw = true;

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
        if (dist > 10.0f) {
            float moveStep = moveSpeed * deltaTime;
            if (moveStep > dist) moveStep = dist;
            cameraX += dx / dist * moveStep;
            cameraY += dy / dist * moveStep;
        }
    }
    cameraX = std::max(0.0f, std::min(cameraX, maxCameraX));
    cameraY = std::max(0.0f, std::min(cameraY, maxCameraY));
    if(Managing.empty()&&Processing==nullptr){
        for(auto unit:Action){
            if(unit->UpdateActionValue(deltaTime)){
                Managing.push(unit);
            }
        }
    }
    
    if(!Managing.empty()&&Processing==nullptr){
        Processing=Managing.front();
        Managing.pop();
        if(std::find(Action.begin(), Action.end(), Processing) == Action.end()){
            Processing=nullptr;
        }
        else{
            drawRadius=true;
            Preview=Processing;
            Preview->drawStep=0;
            cameraTargetX=Processing->gridPos.x*BlockSize-WindowSize.first/2;
            cameraTargetY=Processing->gridPos.y*BlockSize-WindowSize.second/2;
            cameraToTarget=false;
        }
        
        
    }
    //PC行動
    
    if(Processing!=nullptr){
        if (!Processing->IsPlayer()) {
            
            MoveTime += deltaTime;
            if (MoveTime >= 2.0f) {
                Unit* p = Processing;
                cameraTargetX=Processing->gridPos.x*BlockSize-WindowSize.first/2;
                cameraTargetY=Processing->gridPos.y*BlockSize-WindowSize.second/2;
                if(p->Act()){
                    
                }
                else{
                    UnitMoving=true;
                    MoveTime=0;
                }
            }
        }
    }
    
    
    //畫radius
    if(Preview!=nullptr&&drawRadius){
        Preview->UpdateRadiusAnimation(deltaTime);
    }
    //unit移動時間
    if(UnitMoving){
        MoveTime+=deltaTime;
        if(MoveTime>=1.0f){
            Processing->ActionValue=Unit::MaxActionValue/Processing->Speed;
            Processing=nullptr;
            Preview=nullptr;
            
            for(auto obj:UnitGroup->GetObjects()){
                Unit* unit=dynamic_cast<Unit*>(obj);
                unit->calc=false;
            }
            UnitMoving=false;
        }
    }
    //button嘅UI
    if (waitingForConfirm && confirmUnit &&  btnConfirm->Visible) {
        // 1) 取单位世界坐标（格子中心）

        // 2) 转成屏幕坐标（UI 坐标系）
        float screenX = WindowSize.first-2000 - cameraX;
        float screenY = WindowSize.second-1000 - cameraY;
        //cout<<"X:"<<screenX<<" Y:"<<screenY<<endl;
        bx1=WindowSize.first-screenX-520;
        by1 = WindowSize.second-screenY-210;
        bx2 = bx1-1470;
        bx3 = bx1-700;
        btnConfirm->SetPosition(bx1, by1, cameraX, cameraY);
        btnAbilityCancel->SetPosition( bx2, by1, cameraX, cameraY);
        btnAttack->SetPosition(bx3, by1, cameraX, cameraY);

        btnConfirm->Visible = true;
    }
    else {
        btnAbilityCancel->Visible = false;
    }
    //攻擊UI介面
    if(attackUIDraw&&ChooseAbilityDraw) btnAttack->Visible = false;
    else if(!attackUIDraw) btnAttack->Visible = false;
    
    if (attackUIActive) {
        AttackUIVisibleTime -= deltaTime;
        if (AttackUIVisibleTime <= 0.0f) {
            cout<<"click";
            attackUIDraw = false;
            attackUIActive = false;
            btnAttack->Visible = false;
            ChooseAbilityDraw=false;
            waitingForConfirm=false;
            previewSelected = false;
            // 防呆：Processing 失效檢查
            if(Processing && !isUnitInGroup(Processing)) Processing = nullptr;
            if(Processing && Processing->HP<=0){
                RemoveUnit(Processing);
            }
            if(Defense && !isUnitInGroup(Defense)) Defense = nullptr;
            if(Defense && Defense->HP<=0){
                RemoveUnit(Defense);
            }
            // 反擊流程後，若Processing死亡，Defense也可能已被移除，需同步清理
            if(Processing && !isUnitInGroup(Processing)) Processing = nullptr;
            if(Defense && !isUnitInGroup(Defense)) Defense = nullptr;
            Processing=nullptr;
            drawRadius=true;
        }
}
}



void PlayScene::Draw() const {
    al_identity_transform(&Camera);
    al_translate_transform(&Camera, -cameraX, -cameraY);
    al_use_transform(&Camera);
    TileMapGroup->Draw();
    GroundEffectGroup->Draw();
    DebugIndicatorGroup->Draw();
    UnitGroup->Draw();
    EffectGroup->Draw();
    al_identity_transform(&Camera);
    al_use_transform(&Camera);
    if (Preview && !isUnitInGroup(Preview)) {
        // 不能直接賦值給 const 成員，需用 mutable 或在外部處理
        // 這裡直接 return，防止存取失效指標
        return;
    }
    if (Preview != nullptr) {
        if(drawRadius){
            Preview->drawRadius(cameraX, cameraY);
        }
        if(ChooseAbilityDraw){
            ChooseAbilityUI();
        } 
        Preview->DrawUI();
        if(attackUIDraw) AttackUI();
        
    }
    // 顯示所有unit的頭像與行動值於左側（加上格子，值*10並取整數，並由小到大排序，最多5個）
    std::vector<Unit*> sortedAction = Action;
    std::sort(sortedAction.begin(), sortedAction.end(), [](Unit* a, Unit* b) {
        return a->ActionValue < b->ActionValue;
    });
    int y = 100;
    const float iconSize = 120.0f;
    const float cellW = 100.0f;
    const float cellH = 32 + 4;
    int showCount = std::min(10, static_cast<int>(sortedAction.size()));
    DrawActionValue(sortedAction, y, iconSize, cellW, cellH, showCount);
    // 儲存 actionCellRects 供 OnMouseDown 使用
    static std::vector<std::pair<int, Engine::Point>> s_actionCellRects;
    s_actionCellRects = actionCellRects;
    al_identity_transform(&Camera);
    al_translate_transform(&Camera, -cameraX, -cameraY);
    al_use_transform(&Camera);
    UIGroup->Draw();
    al_identity_transform(&Camera);
    al_use_transform(&Camera);
}

void PlayScene::DrawActionValue(const std::vector<Unit*>& sortedAction, int y, float iconSize, float cellW, float cellH, int showCount) const {
    this->actionCellRects.clear();
    for (int i = 0; i < showCount; ++i) {
        Unit* unit = sortedAction[i];
        // 畫格子
        al_draw_filled_rectangle(6, y-2, 6+cellW, y-2+cellH, al_map_rgb(255,255,255));
        al_draw_rectangle(6, y-2, 6+cellW, y-2+cellH, al_map_rgb(180,180,180), 2);
        // 畫頭像
        std::shared_ptr<ALLEGRO_BITMAP> avatarPtr = Engine::Resources::GetInstance().GetBitmap(unit->img);
        ALLEGRO_BITMAP* avatar = avatarPtr.get();
        if (avatar) {
            float srcW = al_get_bitmap_width(avatar);
            float srcH = al_get_bitmap_height(avatar);
            float scale = iconSize / std::max(srcW, srcH);
            float drawW = srcW * scale;
            float drawH = srcH * scale;
            float drawX = 10;
            float drawY = y;
            al_draw_scaled_bitmap(avatar, 100, 50, srcW, srcH, drawX, drawY, drawW, drawH, 0);
        }
        // 畫行動值（*10並取整數）
        int value = static_cast<int>(unit->ActionValue * 10);
        std::string text = std::to_string(value);
        ALLEGRO_FONT* font = font20;
        al_draw_text(font, al_map_rgb(0,0,0), 10 + 32 + 18, y + 4, 0, text.c_str());
        // 儲存格子區域
        this->actionCellRects.push_back({i, Engine::Point(6, y-2)});
        y += 32 + 12;
    }
}

void PlayScene::AttackUI() const{
    if (!Defense || !isUnitInGroup(Defense)) return;
    const float barWidth = 400.0f;
    const float barHeight = 20.0f;
    const float offsetY = 10.0f;
    const float iconSize = 300.0f;
    float healthPercent = static_cast<float>(Defense->MAXHP-Defense->HP) / Defense->MAXHP;
    float filledWidth = barWidth * healthPercent;
    int x=1200;
    int y=0;
    int barX = x-iconSize/5-8;
    // 血條背景（灰色）
    al_draw_filled_rectangle(barX, y + offsetY, barX + barWidth, y + offsetY + barHeight, al_map_rgb(100, 100, 100));
    // 血量（紅色）
    al_draw_filled_rectangle(barX+filledWidth, y + offsetY, barX + barWidth, y + offsetY + barHeight, al_map_rgb(255, 0, 0));
    // 外框（白色）
    al_draw_rectangle(barX, y + offsetY, barX + barWidth, y + offsetY + barHeight, al_map_rgb(255, 255, 255), 1);
    // 頭像顯示（右側，與血條、能量條對齊，保留原比例）
    std::shared_ptr<ALLEGRO_BITMAP> avatarPtr = Engine::Resources::GetInstance().GetBitmap(Defense->img);
    ALLEGRO_BITMAP* avatar = avatarPtr.get();
    if (avatar) {
        float srcW = al_get_bitmap_width(avatar);
        float srcH = al_get_bitmap_height(avatar);
        float scale = iconSize / std::max(srcW, srcH);
        float drawW = srcW * scale;
        float drawH = srcH * scale;
        float drawX = x-160 + barWidth + (iconSize - drawW) / 2-20;
        float drawY = y + offsetY + (iconSize - drawH) / 2-100;
        al_draw_scaled_bitmap(avatar, 0, 0, srcW, srcH, drawX, drawY, drawW, drawH, 1);
    }
    if(!ChooseAbilityDraw)btnAttack->Visible=true;
    char hpText[32];
    snprintf(hpText, sizeof(hpText), "%d / %d", Defense->HP, Defense->MAXHP);
    ALLEGRO_FONT* font = font32;
    int textWidth = al_get_text_width(font, hpText);
    int textHeight = al_get_font_line_height(font);
    al_draw_text(font, al_map_rgb(255,255,255), barX + barWidth/2 - textWidth/2, y + offsetY*4 - textHeight-2, 0, hpText);
    // 顯示能量條
    const float energyBarHeight = 14.0f;
    const float energyOffsetY = offsetY + barHeight;
    float energyPercent = static_cast<float>(Defense->MaxEnergy-Defense->Energy) / static_cast<float>(Defense->MaxEnergy);
    float energyFilledWidth = barWidth * energyPercent;
    al_draw_filled_rectangle(barX, y + energyOffsetY, barX + barWidth, y + energyOffsetY + energyBarHeight, al_map_rgb(60, 60, 180));
    al_draw_filled_rectangle(barX+energyFilledWidth, y + energyOffsetY, barX + barWidth, y + energyOffsetY + energyBarHeight, al_map_rgb(0, 200, 255));
    al_draw_rectangle(barX, y + energyOffsetY, barX + barWidth, y + energyOffsetY + energyBarHeight, al_map_rgb(255, 255, 255), 1);
    // 能量數值
    char energyText[32];
    snprintf(energyText, sizeof(energyText), "%d / %d", Defense->Energy, Defense->MaxEnergy);
    ALLEGRO_FONT* energyFont = font20;
    int energyTextWidth = al_get_text_width(energyFont, energyText);
    int energyTextHeight = al_get_font_line_height(energyFont);
    al_draw_text(energyFont, al_map_rgb(255,255,255), barX + barWidth/2 - energyTextWidth/2, y + energyOffsetY + (energyBarHeight-energyTextHeight)/2, 0, energyText);
    // 顯示本次攻擊與反擊造成的傷害
    if (attackUIActive && (lastAttackDamage > 0 || lastCounterDamage > 0)) {
        ALLEGRO_FONT* bigFont = font48;
        int textY = y + offsetY + barHeight + 40;
        if (lastAttackDamage > 0) {
            al_draw_textf(bigFont, al_map_rgb(255, 0, 0), barX + barWidth/2, textY, ALLEGRO_ALIGN_CENTRE, "%d", lastAttackDamage);
            textY += 56;
        }
        if (lastCounterDamage > 0) {
            al_draw_textf(bigFont, al_map_rgb(255, 80, 80), barX + barWidth/2, textY, ALLEGRO_ALIGN_CENTRE, "%d", lastCounterDamage);
        }
    }
}

void PlayScene::ChooseAbilityUI() const{
    // 畫一層半透明灰色遮罩
    if(Processing->IsPlayer()) btnAbilityCancel->Visible = true;
    else btnAbilityCancel->Visible = false;
    float screenW = Engine::GameEngine::GetInstance().GetScreenSize().x;
    float screenH = Engine::GameEngine::GetInstance().GetScreenSize().y;
    al_draw_filled_rectangle(0, 0, screenW, screenH, al_map_rgba(50, 50, 50, 100));

    const float barWidth = 400.0f;
    const float barHeight = 20.0f;
    const float offsetY = -50.0f;
    const float iconSize = 720.0f;
    int x = 0;
    int y = 0;
    // 頭像顯示（左側，與血條、能量條對齊，保留原比例）
    std::shared_ptr<ALLEGRO_BITMAP> avatarAttackPtr = Engine::Resources::GetInstance().GetBitmap(Processing->img);
    ALLEGRO_BITMAP* avatarAttack = avatarAttackPtr.get();
    if (avatarAttack) {
        float srcW = al_get_bitmap_width(avatarAttack);
        float srcH = al_get_bitmap_height(avatarAttack);
        float scale = iconSize / std::max(srcW, srcH);
        float drawW = srcW * scale;
        float drawH = srcH * scale;
        float drawX = x + (iconSize - drawW) / 2-150;
        float drawY = y + offsetY + (iconSize - drawH) / 2;
        al_draw_scaled_bitmap(avatarAttack, 0, 0, srcW, srcH, drawX, drawY, drawW, drawH, 0);
    }
    std::shared_ptr<ALLEGRO_BITMAP> avatarDefensePtr = Engine::Resources::GetInstance().GetBitmap(Defense->img);
    ALLEGRO_BITMAP* avatarDefense = avatarDefensePtr.get();
    if (avatarDefense) {
        float srcW = al_get_bitmap_width(avatarDefense);
        float srcH = al_get_bitmap_height(avatarDefense);
        float scale = iconSize / std::max(srcW, srcH);
        float drawW = srcW * scale;
        float drawH = srcH * scale;
        float drawX = x + barWidth*2+(iconSize - drawW) / 2+200;
        float drawY = y + offsetY + (iconSize - drawH) / 2;
        al_draw_scaled_bitmap(avatarDefense, 0, 0, srcW, srcH, drawX, drawY, drawW, drawH, 1);
    }
    // 畫技能格（中下方，最多4格，垂直排列，無空隙，底色黑色，更寬，分五格，name最長）
    const int maxSkills = 4;
    const float skillCellW = 1000.0f;
    const float skillCellH = 60.0f;
    const float skillCellGap = 0.0f;
    float totalH = maxSkills * skillCellH + (maxSkills - 1) * skillCellGap;
    float startX = (screenW - skillCellW) / 2;
    float startY = screenH - totalH;

    const std::vector<Skill>& skills = Processing->IsPlayer() ? Processing->GetSkills() : Defense->GetSkills();
    const int colCount = 5;
    // name格最長，其餘四格再長少少
    float nameColW = skillCellW * 0.32f; // 約32%
    float otherColW = (skillCellW - nameColW) / 4.0f; // 其餘四格均分
    // 但再讓其他四格稍微再長一點，nameColW 再縮一點
    nameColW = skillCellW * 0.28f;
    otherColW = (skillCellW - nameColW) / 4.0f;
    float colWs[5] = {nameColW, otherColW, otherColW, otherColW, otherColW};
    float colXs[5];
    colXs[0] = startX;
    for (int c = 1; c < 5; ++c) colXs[c] = colXs[c-1] + colWs[c-1];
    // 標題列
    const char* headers[colCount] = {"Name", "Power", "Range", "Energy", "Crit Rate"};
    ALLEGRO_FONT* headerFont = font22;
    float headerY = startY - skillCellH;
    for (int c = 0; c < colCount; ++c) {
        al_draw_filled_rectangle(colXs[c], headerY, colXs[c] + colWs[c], headerY + skillCellH, al_map_rgb(30, 30, 30));
        al_draw_rectangle(colXs[c], headerY, colXs[c] + colWs[c], headerY + skillCellH, al_map_rgb(180, 180, 180), 2);
        al_draw_text(headerFont, al_map_rgb(255,255,0), colXs[c] + colWs[c]/2, headerY + 14, ALLEGRO_ALIGN_CENTRE, headers[c]);
    }
    // 技能內容
    for (int i = 0; i < maxSkills; ++i) {
        float cellY = startY + i * (skillCellH + skillCellGap);
        for (int c = 0; c < colCount; ++c) {
            al_draw_filled_rectangle(colXs[c], cellY, colXs[c] + colWs[c], cellY + skillCellH, al_map_rgb(0, 0, 0));
            al_draw_rectangle(colXs[c], cellY, colXs[c] + colWs[c], cellY + skillCellH, al_map_rgb(180, 180, 180), 2);
        }
        // 若此行被選中，加粗外框
        if (PlayerselectedSkillIndex == i) {
            al_draw_rectangle(colXs[0], cellY, colXs[4] + colWs[4], cellY + skillCellH, al_map_rgb(255, 255, 0), 5);
        }
        if (i < (int)skills.size()) {
            const auto& skill = skills[i];
            ALLEGRO_FONT* cellFont = font20;
            // 名稱
            al_draw_text(cellFont, al_map_rgb(255,255,255), colXs[0] + colWs[0]/2, cellY + 14, ALLEGRO_ALIGN_CENTRE, skill.name.c_str());
            // Power
            al_draw_textf(cellFont, al_map_rgb(200,255,200), colXs[1] + colWs[1]/2, cellY + 14, ALLEGRO_ALIGN_CENTRE, "%d", skill.power);
            // Range
            al_draw_textf(cellFont, al_map_rgb(200,200,255), colXs[2] + colWs[2]/2, cellY + 14, ALLEGRO_ALIGN_CENTRE, "%d", skill.range);
            // Energy
            al_draw_textf(cellFont, al_map_rgb(255,255,200), colXs[3] + colWs[3]/2, cellY + 14, ALLEGRO_ALIGN_CENTRE, "%d", skill.energy);
            // 暴擊率
            al_draw_textf(cellFont, al_map_rgb(255,200,200), colXs[4] + colWs[4]/2, cellY + 14, ALLEGRO_ALIGN_CENTRE, "%d%%", (int)(skill.critRate*100));
        }
    }
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
        if(!ChooseAbilityDraw)dragging = true;
        cameraToTarget = true;
        dragStartX = mx;
        dragStartY = my;
        cameraStartX = cameraX;
        cameraStartY = cameraY;
        if(Preview==Processing&&drawRadius){
            if(Processing->IsPlayer()){
                if(!(worldX>=bx2-240&&worldX<=bx1+240&&worldY>=by1-80&&worldY<=by1+80)){
                    //cout<<"out"<<endl;
                    Turret *turret = dynamic_cast<Turret *>(Processing);
                    if(turret->CheckPlacement(x, y)){
                        waitingForConfirm=true;
                        confirmUnit   = Processing;
                        confirmTarget = Engine::Point(x, y);
                        btnConfirm->Visible=true;
                    }
                    else{
                        Processing->CancelPreview();
                        drawRadius=false;
                        attackUIDraw=false;

                        btnConfirm->Visible = btnAbilityCancel->Visible = btnAttack->Visible= false;
                    }
                }
                
            }
            else{
                drawRadius=false;
                attackUIDraw=false;
            } 
        }
        else{
            drawRadius=false;
            attackUIDraw=false;
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
    // 點擊左側行動值格子時 Camera 跳到該 unit
    const float cellW = 120.0f;
    const float cellH = 32.0f + 4;
    std::vector<Unit*> sortedAction = Action;
    std::sort(sortedAction.begin(), sortedAction.end(), [](Unit* a, Unit* b) {
        return a->ActionValue < b->ActionValue;
    });
    int showCount = std::min(10, static_cast<int>(sortedAction.size()));
    for (int i = 0; i < showCount; ++i) {
        Engine::Point cellLT = actionCellRects[i].second;
        int x1 = cellLT.x, y1 = cellLT.y;
        int x2 = x1 + cellW, y2 = y1 + cellH;
        if (mx >= x1 && mx <= x2 && my >= y1 && my <= y2) {
            Unit* unit = sortedAction[i];
            cameraTargetX = unit->gridPos.x * BlockSize - WindowSize.first / 2;
            cameraTargetY = unit->gridPos.y * BlockSize - WindowSize.second / 2;
            cameraToTarget = false;
            return;
        }
    }
    // 技能選擇（ChooseAbilityUI）
    if (ChooseAbilityDraw) {
        // 計算技能格範圍
        const int maxSkills = 4;
        const float skillCellW = 1000.0f;
        const float skillCellH = 60.0f;
        float totalH = maxSkills * skillCellH;
        float startX = (Engine::GameEngine::GetInstance().GetScreenSize().x - skillCellW) / 2;
        float startY = Engine::GameEngine::GetInstance().GetScreenSize().y - totalH;
        float nameColW = skillCellW * 0.28f;
        float otherColW = (skillCellW - nameColW) / 4.0f;
        float colWs[5] = {nameColW, otherColW, otherColW, otherColW, otherColW};
        float colXs[5];
        colXs[0] = startX;
        for (int c = 1; c < 5; ++c) colXs[c] = colXs[c-1] + colWs[c-1];
        int mx_ = mx, my_ = my;
        for (int i = 0; i < maxSkills; ++i) {
            float cellY = startY + i * skillCellH;
            // 只有有技能的格子才可被選擇
            int skillSize;
            Unit* Player;
            if(Processing->IsPlayer()){
                Player=Processing;
            }
            else Player=Defense;
            skillSize = Player->GetSkills().size();
            if (i < skillSize && Player->Energy>= Player->GetSkills()[i].energy) {
                if (mx_ >= colXs[0] && mx_ <= colXs[4] + colWs[4] && my_ >= cellY && my_ <= cellY + skillCellH) {
                    PlayerselectedSkillIndex = i;
                    return;
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
    float screenW = Engine::GameEngine::GetInstance().GetScreenSize().x;
    float screenH = Engine::GameEngine::GetInstance().GetScreenSize().y;
    btnConfirm = new Engine::ImageButton("play/Idle.png", "play/Hover.png",200, 236, 120, 40);
    btnConfirm->SetOnClickCallback(std::bind(&PlayScene::ConfirmClick, this)) ;   
    AddNewControlObject(btnConfirm);                                 
    btnAbilityCancel = new Engine::ImageButton("play/Idle.png",  "play/Hover.png",0, 0, 120, 40);
    btnAbilityCancel->SetOnClickCallback(std::bind(&PlayScene::CancelClick, this)) ;  
    AddNewControlObject(btnAbilityCancel);  
    btnAttack = new Engine::ImageButton("play/Idle.png",  "play/Hover.png",0, 0, 120, 40);
    btnAttack->SetOnClickCallback(std::bind(&PlayScene::AttackClick, this)) ;  
    AddNewControlObject(btnAttack);
    btnConfirm->Visible = btnAbilityCancel->Visible = btnAttack->Visible=false;

    UIGroup->AddNewObject(btnConfirm);
    UIGroup->AddNewObject(btnAbilityCancel);
    UIGroup->AddNewObject(btnAttack);

    // 載入攻擊傷害顯示字型
    if (!attackUIFont) {
        attackUIFont = al_load_ttf_font("Resource/fonts/pirulen.ttf", 28, 0);
        if (!attackUIFont) attackUIFont = al_create_builtin_font();
    }
    // 統一載入所有 UI 會用到的字型
    if (!font20) font20 = al_load_ttf_font("Resource/fonts/pirulen.ttf", 20, 0);
    if (!font22) font22 = al_load_ttf_font("Resource/fonts/pirulen.ttf", 22, 0);
    if (!font28) font28 = al_load_ttf_font("Resource/fonts/pirulen.ttf", 28, 0);
    if (!font32) font32 = al_load_ttf_font("Resource/fonts/pirulen.ttf", 32, 0);
    if (!font48) font48 = al_load_ttf_font("Resource/fonts/pirulen.ttf", 48, 0);
    // fallback
    if (!font20) font20 = al_create_builtin_font();
    if (!font22) font22 = al_create_builtin_font();
    if (!font28) font28 = al_create_builtin_font();
    if (!font32) font32 = al_create_builtin_font();
    if (!font48) font48 = al_create_builtin_font();
}

void PlayScene::SetDrawRadius(bool value) {
    drawRadius = value;
}

void PlayScene::RemoveUnit(Unit* unit) {
    if (!unit) return;
    // 從 UnitGroup 移除
    UnitGroup->RemoveObjectByInstance(unit);
    // 從 Action 移除
    Action.erase(std::remove(Action.begin(), Action.end(), unit), Action.end());
    // 清理 Processing/Defense/Preview 指標
    if (Processing == unit) Processing = nullptr;
    if (Defense == unit) Defense = nullptr;
    if (Preview == unit) Preview = nullptr;
}



