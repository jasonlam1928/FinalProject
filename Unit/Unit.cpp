#include "Unit.hpp"
#include<iostream>
#include "Scene/PlayScene.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Resources.hpp"
#include <allegro5/allegro_primitives.h>

using Engine::IntPoint;
using Engine::Resources;


PlayScene* Unit::getPlayScene() {
    return dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetActiveScene());
}

Unit::Unit(float x, float y,std::string base, std::string img, std::string fight_img, float speed, int hp, int distance, int damage, int energy, int attackRange, std::string Label)
    : Sprite(img, x, y),imgBase(base, x, y),fight_img(fight_img),Speed(speed), HP(hp), distance(distance), calc(false), damage(damage), MaxEnergy(energy),Energy(energy/4), Label(Label), ActionValue(MaxActionValue/speed), img(img), attackRange(attackRange) {
    gridPos = IntPoint(x / PlayScene::BlockSize, y / PlayScene::BlockSize);
    MAXHP=HP;
}

bool Unit::UpdateActionValue(float deltaTime) {
    ActionValue -=deltaTime*5;
    //cout<<Speed<<""<<deltaTime<<endl;
    if (ActionValue <= 0) {
        ActionValue=0;
        return true;
    }
    return false;
}

void Unit::DrawUI(){
    const float barWidth = 400.0f;
    const float barHeight = 20.0f;
    const float offsetY = 10.0f;
    const float iconSize = 45.0f;
    float healthPercent = static_cast<float>(HP) / static_cast<float>(MAXHP);
    float filledWidth = barWidth * healthPercent;
    int x = 0;
    int y = 0;
    // 頭像顯示（左側，與血條、能量條對齊，保留原比例）
    std::shared_ptr<ALLEGRO_BITMAP> avatarPtr = Engine::Resources::GetInstance().GetBitmap(img);
    ALLEGRO_BITMAP* avatar = avatarPtr.get();
    if (avatar) {
        float srcW = al_get_bitmap_width(avatar);
        float srcH = al_get_bitmap_height(avatar);
        float scale = iconSize / std::max(srcW, srcH);
        float drawW = srcW * scale;
        float drawH = srcH * scale;
        float drawX = x + (iconSize - drawW) / 2;
        float drawY = y + offsetY + (iconSize - drawH) / 2;
        al_draw_scaled_bitmap(avatar, 0, 0, srcW, srcH, drawX, drawY, drawW, drawH, 0);
    }
    int barX = x + iconSize + 8; // 血條與能量條右移，預留頭像空間
    // 血條背景（灰色）
    al_draw_filled_rectangle(barX, y + offsetY, barX + barWidth, y + offsetY + barHeight, al_map_rgb(100, 100, 100));
    // 血量（紅色）
    al_draw_filled_rectangle(barX, y + offsetY, barX + filledWidth, y + offsetY + barHeight, al_map_rgb(255, 0, 0));
    // 外框（白色）
    al_draw_rectangle(barX, y + offsetY, barX + barWidth, y + offsetY + barHeight, al_map_rgb(255, 255, 255), 1);
    // 顯示血量數值
    char hpText[32];
    snprintf(hpText, sizeof(hpText), "%d / %d", HP, MAXHP);
    ALLEGRO_FONT* hpfont = al_load_ttf_font("Resource/fonts/pirulen.ttf", 32, 0);
    if (!hpfont) hpfont = al_create_builtin_font();
    int hptextWidth = al_get_text_width(hpfont, hpText);
    int hptextHeight = al_get_font_line_height(hpfont);
    al_draw_text(hpfont, al_map_rgb(255,255,255), barX + barWidth/2 - hptextWidth/2, y + offsetY*4 - hptextHeight-2, 0, hpText);
    al_destroy_font(hpfont);
    // 能量條
    const float energyBarHeight = 14.0f;
    const float energyOffsetY = offsetY + barHeight;
    float energyPercent = static_cast<float>(Energy) / static_cast<float>(MaxEnergy);
    float energyFilledWidth = barWidth * energyPercent;
    al_draw_filled_rectangle(barX, y + energyOffsetY, barX + barWidth, y + energyOffsetY + energyBarHeight, al_map_rgb(60, 60, 180));
    al_draw_filled_rectangle(barX, y + energyOffsetY, barX+energyFilledWidth, y + energyOffsetY + energyBarHeight, al_map_rgb(0, 200, 255));
    al_draw_rectangle(barX, y + energyOffsetY, barX + barWidth, y + energyOffsetY + energyBarHeight, al_map_rgb(255, 255, 255), 1);
    // 能量數值
    char energyText[32];
    snprintf(energyText, sizeof(energyText), "%d / %d", Energy, MaxEnergy);
    ALLEGRO_FONT* energyFont = al_load_ttf_font("Resource/fonts/pirulen.ttf", 18, 0);
    if (!energyFont) energyFont = al_create_builtin_font();
    int energyTextWidth = al_get_text_width(energyFont, energyText);
    int energyTextHeight = al_get_font_line_height(energyFont);
    al_draw_text(energyFont, al_map_rgb(255,255,255), barX + barWidth/2 - energyTextWidth/2, y + energyOffsetY + (energyBarHeight-energyTextHeight)/2, 0, energyText);
    al_destroy_font(energyFont);
}

void Unit::Reset(){
    ActionValue = MaxActionValue / Speed;
    Energy+=MaxEnergy/10;
    if (Energy > MaxEnergy) {
        Energy = MaxEnergy;
    }
    for(auto obj:getPlayScene()->UnitGroup->GetObjects()){
        Unit* unit = dynamic_cast<Unit*>(obj);

        unit->calc=false;
    }
    
}


void Unit::Update(float deltaTime){
    
}


void Unit::drawRadius(int cameraX, int cameraY) {
    if (!calc) {
        int dx[4] = {0, 0, 1, -1};
        int dy[4] = {1, -1, 0, 0};

        std::queue<IntPoint> q;
        std::set<IntPoint> visited;
        std::map<IntPoint, int> level;
        std::map<IntPoint, bool> valid;

        q.push(gridPos);
        visited.insert(gridPos);
        level[gridPos] = 0;
        valid[gridPos] = true;

        int step = 0;
        while (!q.empty() && step < distance+attackRange) {
            int sz = q.size();
            for (int i = 0; i < sz; ++i) {
                IntPoint cur = q.front(); q.pop();
                for (int d = 0; d < 4; ++d) {
                    IntPoint nxt = cur + IntPoint(dx[d], dy[d]);
                    bool outOfRange = false;
                    if (visited.count(nxt)) continue;
                    // 修正：用 enum 判斷 TILE_SAND
                    if(getPlayScene()->mapState[nxt.y][nxt.x] == PlayScene::TILE_SAND){
                        valid[nxt]=false;
                        outOfRange = true;
                    }

                    if (nxt.x <= 1 || nxt.x >= 62 || nxt.y <= 1 || nxt.y >= 62) {
                        outOfRange = true;
                        valid[nxt] = false;
                    }
                    else if (step >= distance) {
                        outOfRange = true;
                        valid[nxt] = false;
                        q.push(nxt);
                    }

                    for (auto& obj : getPlayScene()->UnitGroup->GetObjects()) {
                        auto unit = dynamic_cast<Unit*>(obj);
                        if (nxt == unit->gridPos) {
                            valid[nxt] = false;
                            outOfRange=true;
                            q.push(nxt);
                            break;
                        }
                    }

                    visited.insert(nxt);
                    level[nxt] = step + 1;
                    if (!outOfRange) {
                        q.push(nxt);
                        valid[nxt] = true;
                    }
                }
            }
            ++step;
        }

        radius = visited;
        radiusStep = level;
        MoveValid = valid;
        drawStep = 0;
        drawTimer = 0;
        calc = true;
    }

    for (auto& r : radius) {
        int dist = radiusStep[r];
        if (dist > drawStep) continue;

        float x = r.x * PlayScene::BlockSize - cameraX;
        float y = r.y * PlayScene::BlockSize - cameraY;
        ALLEGRO_COLOR fillColor = MoveValid[r] ?
            al_map_rgba(144, 238, 144, 120) :
            al_map_rgba(238, 144, 144, 200);
        al_draw_filled_rectangle(x, y, x + PlayScene::BlockSize, y + PlayScene::BlockSize, fillColor);
        al_draw_rectangle(x, y, x + PlayScene::BlockSize, y + PlayScene::BlockSize, al_map_rgb(0, 128, 0), 1);
    }
}

void Unit::UpdateRadiusAnimation(float deltaTime) {
    drawTimer += deltaTime;
    if (drawTimer >= drawInterval) {
        drawStep++;
        drawTimer = 0;
    }
}

void Unit::UnitHit(float UnitDamage){
    HP-=static_cast<int>(UnitDamage);
    if(HP<=0)HP=0;
    
}

void Unit::MovetoPreview() {
    gridPos = previewPos;
    calc=false;
}

void Unit::CancelPreview() {
    Sprite::Move(gridPos.x * PlayScene::BlockSize + PlayScene::BlockSize/2, gridPos.y * PlayScene::BlockSize + PlayScene::BlockSize/2);
}

void Unit::AddSkill(const Skill& skill) {
    if (skills.size() < 4)
        skills.push_back(skill);
}

const std::vector<Skill>& Unit::GetSkills() const {
    return skills;
}

