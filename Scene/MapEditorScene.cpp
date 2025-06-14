#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/drawing.h>
#include <functional>
#include <memory>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <iostream>

#include "Unit/Unit.hpp"
#include "Unit/Enemy/Enemy.hpp"
#include "Unit/Enemy/EnemyKnight1.hpp"
#include "Unit/Enemy/EnemyGunner1.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Group.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Component/ImageButton.hpp"
#include "MapEditorScene.hpp"

#include "Unit/Player/Player.hpp"
#include "Unit/Player/Knight1.hpp"
#include "Unit/Player/Gunner1.hpp"

int MapWidth = 20, MapHeight = 15; // 預設值，會被檔案覆蓋
const int BlockSize = 64;

void MapEditorScene::Initialize() {
    std::cout << "[DEBUG] MapEditorScene::Initialize called" << std::endl;
    cameraX = cameraY = 0;
    dragging = false;
    AddNewObject(TileMapGroup = new Engine::Group());
    AddNewObject(EnemyGroup = new Engine::Group());

    // Initialize tileTypes before using it
    tileTypes = {
        "play/GrassTile.png",   // 0
        "play/PlacableTile.png", // 1
        "play/Rock.png",
        "play/MiniRock.png"  // 2 (or another image if you have more types)
    };

    // Load map (reuse PlayScene logic)
    std::string filename = std::string("Resource/map") + std::to_string(MapId) + ".txt";
    std::cout << "{Initialising MapEditor} Loading map from: " << filename << std::endl;
    char c;

    // 先讀取地圖寬高
    int fileWidth = 0, fileHeight = 0;
    std::ifstream fin(filename);
    if (fin.is_open()) {
        fin >> fileWidth >> fileHeight>>knightCount>>gunnerCount>>maxUnit;
        fin.get(); // 跳過換行
        MapWidth = fileWidth;
        MapHeight = fileHeight;
    }
    // 依據檔案 size 配置 mapState
    mapState = std::vector<std::vector<int>>(MapHeight, std::vector<int>(MapWidth));
    std::vector<int> tileData;
    for (int i = 0; i < MapHeight; ++i) {
        for (int j = 0; j < MapWidth; ++j) {
            fin >> c;
            if (c == '\n' || c == '\r') {
                --j; // 跳過換行符
                continue;
            }
            tileData.push_back(c - '0');
            mapState[i][j] = c - '0';
        }
    }
    fin.close();
    // 根據 tileData 建立 TileMapGroup
    for (int i = 0; i < MapHeight; i++) {
        for (int j = 0; j < MapWidth; j++) {
            int num = mapState[i][j];
            if (num >= 0 && num < static_cast<int>(tileTypes.size()))
                TileMapGroup->AddNewObject(new Engine::Image(tileTypes[num], j * BlockSize, i * BlockSize, BlockSize, BlockSize));
        }
    }

    // Read unit data into a vector of tuples
    std::string enemyFilename = std::string("Resource/enemy") + std::to_string(MapId) + ".txt";
    unitData.clear();
    std::ifstream enemyFin(enemyFilename);
    float type, x, y;
    while (enemyFin >> type && enemyFin >> x && enemyFin >> y) {
        unitData.emplace_back(static_cast<int>(type), x, y);
    }
    enemyFin.close();

    for (const auto& [typeInt, ux, uy] : unitData) {
        float posX = (ux - 1) * BlockSize - BlockSize / 2;
        float posY = (uy - 1) * BlockSize - BlockSize / 2;
        Engine::Sprite* enemy = nullptr;
        switch (typeInt) {
            case 1:
                enemy = new EnemyKnight1(posX, posY);
                break;
            case 2:
                enemy = new Knight1(posX, posY);
                break;
            case 3:
                enemy = new EnemyGunner1(posX, posY);
                break;
            case 4:
                enemy = new Gunner1(posX, posY);
                break;
        }
        if (enemy) {
            EnemyGroup->AddNewObject(enemy);
        }
    }

    AddNewObject(new Engine::Label("Map Editor", "pirulen.ttf", 48, 400, 50, 255, 255, 255, 255, 0.5, 0.5));
}

void MapEditorScene::Terminate() {
    IScene::Terminate();
}

void MapEditorScene::Update(float deltaTime) {
    // Only camera movement, no gameplay
    if (dragging) {
        float screenW = Engine::GameEngine::GetInstance().GetScreenSize().x;
        float screenH = Engine::GameEngine::GetInstance().GetScreenSize().y;      

        float reservedWidth = screenW * 0.5f; /*右邊要讓嚟放 Map Assets*/
        float reservedSpace = screenW * 0.1f; 

        cameraX = std::max(0.0f - reservedSpace, std::min(cameraX, float(MapWidth * BlockSize - (screenW - reservedWidth))));
        cameraY = std::max(0.0f - reservedSpace, std::min(cameraY, float(MapHeight * BlockSize - (screenH - reservedSpace))));
    }
}

void MapEditorScene::Draw() const {
    ALLEGRO_TRANSFORM Camera;
    al_identity_transform(&Camera);
    al_translate_transform(&Camera, -cameraX, -cameraY);
    al_use_transform(&Camera);
    al_clear_to_color(al_map_rgb(0, 0, 0));
    // Draw map tiles from mapState/tileData
    for (int y = 0; y < MapHeight; ++y) {
        for (int x = 0; x < MapWidth; ++x) {
            int num = mapState[y][x];
            std::string tilePath = (num >= 0 && num < static_cast<int>(tileTypes.size())) ? tileTypes[num] : "play/dirt.png";
            ALLEGRO_BITMAP* bmp = Engine::Resources::GetInstance().GetBitmap(tilePath).get();
            if (bmp) {
                al_draw_bitmap(bmp, x * BlockSize, y * BlockSize, 0);
            }
        }
    }
    // Draw units from unitData
    for (const auto& unit : unitData) {
        int type = std::get<0>(unit);
        float ux = std::get<1>(unit);
        float uy = std::get<2>(unit);
        float posX = (ux - 1) * BlockSize + BlockSize / 2;
        float posY = (uy - 1) * BlockSize + BlockSize / 2;
        std::string spritePath;
        Engine::Sprite* imgBase;
        switch (type) {
            case 1:
                spritePath = "play/ShieldIcon.png";
                imgBase = new Engine::Sprite("play/EnemyBase.png", posX, posY);
                break;
            case 2:
                spritePath = "play/ShieldIcon.png";
                imgBase = new Engine::Sprite("play/PlayerBase.png", posX, posY);
                break;
            case 3:
                spritePath = "play/GunnerIcon.png";
                imgBase = new Engine::Sprite("play/EnemyBase.png", posX, posY);
                break;
            case 4:
                spritePath = "play/GunnerIcon.png";
                imgBase = new Engine::Sprite("play/PlayerBase.png", posX, posY);
                break;
        }
        ALLEGRO_BITMAP* bmp = Engine::Resources::GetInstance().GetBitmap(spritePath).get();
        if (bmp) {
            imgBase->Draw();
            float bmpW = al_get_bitmap_width(bmp);
            float bmpH = al_get_bitmap_height(bmp);
            float drawW = bmpW ;
            float drawH = bmpH ;
            al_draw_scaled_bitmap(
                bmp,
                0, 0,
                bmpW, bmpH,
                posX - drawW / 2.0f, posY - drawH / 2.0f,
                drawW, drawH,
                0
            );
        
        }
    }
    // Reset transform for UI
    al_identity_transform(&Camera);
    al_use_transform(&Camera);
    float screenW = Engine::GameEngine::GetInstance().GetScreenSize().x;
    float screenH = Engine::GameEngine::GetInstance().GetScreenSize().y;
    float toolbarSize = screenW * 0.4f;
    al_draw_filled_rectangle(screenW - toolbarSize, 0, screenW, screenH, al_map_rgb(40, 40, 40));
    // Toolbar background
    al_draw_filled_rectangle(screenW - toolbarSize, 0, screenW, screenH, al_map_rgb(40, 40, 40));
    // Tab buttons
    float tabHeight = 40;
    float tabWidth = toolbarSize / 2;
    ALLEGRO_COLOR mapTabColor = (currentSection == SECTION_MAP) ? al_map_rgb(80, 120, 200) : al_map_rgb(80, 80, 80);
    ALLEGRO_COLOR enemyTabColor = (currentSection == SECTION_UNIT) ? al_map_rgb(80, 120, 200) : al_map_rgb(80, 80, 80);
    ALLEGRO_COLOR PlayerTabColor = (currentSection == SECTION_PLAYER) ? al_map_rgb(80, 120, 200) : al_map_rgb(80, 80, 80);
    float thirdTabWidth = toolbarSize / 3;
    al_draw_filled_rectangle(screenW - toolbarSize, 0, screenW - toolbarSize + thirdTabWidth, tabHeight, mapTabColor);
    al_draw_filled_rectangle(screenW - toolbarSize + thirdTabWidth, 0, screenW - toolbarSize + 2 * thirdTabWidth, tabHeight, enemyTabColor);
    al_draw_filled_rectangle(screenW - toolbarSize + 2 * thirdTabWidth, 0, screenW, tabHeight, PlayerTabColor);
    // Draw tab labels, evenly divided into three sections
    al_draw_text(al_create_builtin_font(), al_map_rgb(255,255,255), screenW - toolbarSize + thirdTabWidth / 2, 10, ALLEGRO_ALIGN_CENTRE, "Map");
    al_draw_text(al_create_builtin_font(), al_map_rgb(255,255,255), screenW - toolbarSize + thirdTabWidth + thirdTabWidth / 2, 10, ALLEGRO_ALIGN_CENTRE, "Enemy");
    al_draw_text(al_create_builtin_font(), al_map_rgb(255,255,255), screenW - toolbarSize + 2 * thirdTabWidth + thirdTabWidth / 2, 10, ALLEGRO_ALIGN_CENTRE, "Player");
    // Section content
    if (currentSection == SECTION_MAP) {
        int removeBtnIndex = static_cast<int>(tileTypes.size());
        for (size_t i = 0; i < tileTypes.size(); ++i) {
            float iconX = screenW - toolbarSize + 20 + i * (BlockSize + 10);
            float iconY = tabHeight + 40;
            al_draw_bitmap(Engine::Resources::GetInstance().GetBitmap(tileTypes[i]).get(), iconX, iconY, 0);
            if (i == selectedTileIndex) {
                al_draw_rectangle(iconX, iconY, iconX + BlockSize, iconY + BlockSize, al_map_rgb(255, 255, 0), 3);
            }
        }
        // Draw Remove Tile as a selectable tile
        float iconX = screenW - toolbarSize + 20 + removeBtnIndex * (BlockSize + 10);
        float iconY = tabHeight + 40;
        al_draw_filled_rectangle(iconX, iconY, iconX + BlockSize, iconY + BlockSize, al_map_rgb(180, 60, 60));
        al_draw_text(al_create_builtin_font(), al_map_rgb(255,255,255), iconX + BlockSize/2, iconY + BlockSize/2 - 10, ALLEGRO_ALIGN_CENTRE, "Remove");
        if (removeButtonSelected) {
            al_draw_rectangle(iconX, iconY, iconX + BlockSize, iconY + BlockSize, al_map_rgb(255, 255, 0), 3);
        }
    } else if (currentSection == SECTION_UNIT) {
        int removeBtnIndex = static_cast<int>(enemyTypes.size());
        for (size_t i = 0; i < enemyTypes.size(); ++i) {
            float iconX = screenW - toolbarSize + 20 + i * (BlockSize + 10);
            float iconY = tabHeight + 40;
            ALLEGRO_COLOR baseColor = al_map_rgb(255, 0, 0);
            std::string spritePath = "play/Knight_01.png";
            switch (enemyTypes[i]) {
                case 1:
                    spritePath = "play/ShieldIcon.png";
                    baseColor = al_map_rgb(255, 0, 0);
                    break;
                case 2:
                    spritePath = "play/ShieldIcon.png";
                    baseColor = al_map_rgb(0, 255, 0);
                    break;
                case 3:
                    spritePath = "play/GunnerIcon.png";
                    baseColor = al_map_rgb(255, 0, 0);
                    break;
                case 4:
                    spritePath = "play/GunnerIcon.png";
                    baseColor = al_map_rgb(0, 255, 0);
            }
            al_draw_filled_rectangle(iconX, iconY, iconX + BlockSize, iconY + BlockSize, baseColor);
            ALLEGRO_BITMAP* bmp = Engine::Resources::GetInstance().GetBitmap(spritePath).get();
            if (bmp) {
                float bmpW = al_get_bitmap_width(bmp);
                float bmpH = al_get_bitmap_height(bmp);
                float drawW = bmpW / 2.0f;
                float drawH = bmpH / 2.0f;
                float centerX = iconX + (BlockSize - drawW) / 2.0f;
                float centerY = iconY + (BlockSize - drawH) / 2.0f;
                al_draw_scaled_bitmap(
                    bmp,
                    0, 0,
                    bmpW, bmpH,
                    centerX, centerY,
                    drawW, drawH,
                    0
                );
            }
            if (i == selectedEnemyIndex) {
                al_draw_rectangle(iconX, iconY, iconX + BlockSize, iconY + BlockSize, al_map_rgb(255, 255, 0), 3);
            }
        }
        // Draw Remove Unit as a selectable tile
        float iconX = screenW - toolbarSize + 20 + removeBtnIndex * (BlockSize + 10);
        float iconY = tabHeight + 40;
        al_draw_filled_rectangle(iconX, iconY, iconX + BlockSize, iconY + BlockSize, al_map_rgb(60, 60, 180));
        al_draw_text(al_create_builtin_font(), al_map_rgb(255,255,255), iconX + BlockSize/2, iconY + BlockSize/2 - 10, ALLEGRO_ALIGN_CENTRE, "Remove");
        if (removeButtonSelected) {
            al_draw_rectangle(iconX, iconY, iconX + BlockSize, iconY + BlockSize, al_map_rgb(255, 255, 0), 3);
        }
    }
    else if(currentSection == SECTION_PLAYER){
        // 顯示 knight/gunner 數量與加減按鈕
        ALLEGRO_FONT* font = al_create_builtin_font();
        float baseY = tabHeight + 60;
        float iconX = screenW - toolbarSize + 40;
        float iconY = baseY;
        float gapY = 80;
        // Knight
        ALLEGRO_BITMAP* knightBmp = Engine::Resources::GetInstance().GetBitmap("play/ShieldIcon.png").get();
        if(knightBmp)
            al_draw_scaled_bitmap(knightBmp, 0,0,al_get_bitmap_width(knightBmp),al_get_bitmap_height(knightBmp), iconX, iconY, 48, 48, 0);
        al_draw_text(font, al_map_rgb(255,255,255), iconX+60, iconY+12, 0, "Knight");
        al_draw_textf(font, al_map_rgb(255,255,0), iconX+160, iconY+12, 0, "%d", knightCount);
        // + - 按鈕
        float btnW=32, btnH=32;
        float plusX=iconX+200, minusX=iconX+240, btnY=iconY+8;
        al_draw_filled_rectangle(plusX, btnY, plusX+btnW, btnY+btnH, al_map_rgb(60,180,60));
        al_draw_text(font, al_map_rgb(255,255,255), plusX+btnW/2, btnY+4, ALLEGRO_ALIGN_CENTRE, "+");
        al_draw_filled_rectangle(minusX, btnY, minusX+btnW, btnY+btnH, al_map_rgb(180,60,60));
        al_draw_text(font, al_map_rgb(255,255,255), minusX+btnW/2, btnY+4, ALLEGRO_ALIGN_CENTRE, "-");
        // Gunner
        iconY += gapY;
        ALLEGRO_BITMAP* gunnerBmp = Engine::Resources::GetInstance().GetBitmap("play/GunnerIcon.png").get();
        if(gunnerBmp)
            al_draw_scaled_bitmap(gunnerBmp, 0,0,al_get_bitmap_width(gunnerBmp),al_get_bitmap_height(gunnerBmp), iconX, iconY, 48, 48, 0);
        al_draw_text(font, al_map_rgb(255,255,255), iconX+60, iconY+12, 0, "Gunner");
        al_draw_textf(font, al_map_rgb(255,255,0), iconX+160, iconY+12, 0, "%d", gunnerCount);
        float plusX2=iconX+200, minusX2=iconX+240, btnY2=iconY+8;
        al_draw_filled_rectangle(plusX2, btnY2, plusX2+btnW, btnY2+btnH, al_map_rgb(60,180,60));
        al_draw_text(font, al_map_rgb(255,255,255), plusX2+btnW/2, btnY2+4, ALLEGRO_ALIGN_CENTRE, "+");
        al_draw_filled_rectangle(minusX2, btnY2, minusX2+btnW, btnY2+btnH, al_map_rgb(180,60,60));
        al_draw_text(font, al_map_rgb(255,255,255), minusX2+btnW/2, btnY2+4, ALLEGRO_ALIGN_CENTRE, "-");
        // maxUnit + -
        iconY += gapY;
        ALLEGRO_BITMAP* baseBmp = Engine::Resources::GetInstance().GetBitmap("play/EnemyBase.png").get();
        if(baseBmp)
            al_draw_scaled_bitmap(baseBmp, 0,0,al_get_bitmap_width(baseBmp),al_get_bitmap_height(baseBmp), iconX, iconY, 48, 48, 0);
        al_draw_text(font, al_map_rgb(255,255,255), iconX+60, iconY+12, 0, "Max Unit");
        al_draw_textf(font, al_map_rgb(0,255,255), iconX+160, iconY+12, 0, "%d", maxUnit);
        float plusX3=iconX+200, minusX3=iconX+240, btnY3=iconY+8;
        al_draw_filled_rectangle(plusX3, btnY3, plusX3+btnW, btnY3+btnH, al_map_rgb(60,180,60));
        al_draw_text(font, al_map_rgb(255,255,255), plusX3+btnW/2, btnY3+4, ALLEGRO_ALIGN_CENTRE, "+");
        al_draw_filled_rectangle(minusX3, btnY3, minusX3+btnW, btnY3+btnH, al_map_rgb(180,60,60));
        al_draw_text(font, al_map_rgb(255,255,255), minusX3+btnW/2, btnY3+4, ALLEGRO_ALIGN_CENTRE, "-");
    }
    // Draw Save button at the bottom of the toolbar
    float saveBtnW = 120, saveBtnH = 48;
    float saveBtnX = screenW - saveBtnW - 20;
    float saveBtnY = screenH - saveBtnH - 20;
    al_draw_filled_rectangle(saveBtnX, saveBtnY, saveBtnX + saveBtnW, saveBtnY + saveBtnH, al_map_rgb(60, 180, 60));
    al_draw_rectangle(saveBtnX, saveBtnY, saveBtnX + saveBtnW, saveBtnY + saveBtnH, al_map_rgb(255,255,255), 2);
    al_draw_text(al_create_builtin_font(), al_map_rgb(255,255,255), saveBtnX + saveBtnW/2, saveBtnY + saveBtnH/2 - 10, ALLEGRO_ALIGN_CENTRE, "Save");

    // Cancel button (same style, left of Save)
    float cancelBtnW = 120, cancelBtnH = 48;
    float cancelBtnX = saveBtnX - cancelBtnW - 20;
    float cancelBtnY = saveBtnY;
    al_draw_filled_rectangle(cancelBtnX, cancelBtnY, cancelBtnX + cancelBtnW, cancelBtnY + cancelBtnH, al_map_rgb(180, 60, 60));
    al_draw_rectangle(cancelBtnX, cancelBtnY, cancelBtnX + cancelBtnW, cancelBtnY + cancelBtnH, al_map_rgb(255,255,255), 2);
    al_draw_text(al_create_builtin_font(), al_map_rgb(255,255,255), cancelBtnX + cancelBtnW/2, cancelBtnY + cancelBtnH/2 - 10, ALLEGRO_ALIGN_CENTRE, "Cancel");


}

void MapEditorScene::OnMouseDown(int button, int mx, int my) {
    float screenW = Engine::GameEngine::GetInstance().GetScreenSize().x;
    float toolbarSize = screenW * 0.4f;
    float tabHeight = 40;
    float tabWidth = toolbarSize / 2;
    float tabStartX = screenW - toolbarSize;
    float tabEndX = screenW;
    float iconY = tabHeight + 40;

    if (currentSection == SECTION_MAP) {
        for (size_t i = 0; i < tileTypes.size(); ++i) {
            float iconX = screenW - toolbarSize + 20 + i * (BlockSize + 10);
            if (mx >= iconX && mx <= iconX + BlockSize && my >= iconY && my <= iconY + BlockSize) {
                selectedTileIndex = static_cast<int>(i);
                removeButtonSelected = false;
                return;
            }
        }
        // Remove tile button as tile
        int removeBtnIndex = static_cast<int>(tileTypes.size());
        float iconX = screenW - toolbarSize + 20 + removeBtnIndex * (BlockSize + 10);
        if (mx >= iconX && mx <= iconX + BlockSize && my >= iconY && my <= iconY + BlockSize) {
            selectedTileIndex = -1;
            removeButtonSelected = true;
            return;
        }
    } else if (currentSection == SECTION_UNIT) {
        for (size_t i = 0; i < enemyTypes.size(); ++i) {
            float iconX = screenW - toolbarSize + 20 + i * (BlockSize + 10);
            if (mx >= iconX && mx <= iconX + BlockSize && my >= iconY && my <= iconY + BlockSize) {
                selectedEnemyIndex = static_cast<int>(i);
                removeButtonSelected = false;
                return;
            }
        }
        // Remove unit button as tile
        int removeBtnIndex = static_cast<int>(enemyTypes.size());
        float iconX = screenW - toolbarSize + 20 + removeBtnIndex * (BlockSize + 10);
        if (mx >= iconX && mx <= iconX + BlockSize && my >= iconY && my <= iconY + BlockSize) {
            selectedEnemyIndex = -1;
            removeButtonSelected = true;
            return;
        }
    }
    else if(currentSection == SECTION_PLAYER){
        // Knight + -
        float baseY = tabHeight + 60;
        float iconX = screenW - toolbarSize + 40;
        float iconY = baseY;
        float btnW=32, btnH=32;
        float plusX=iconX+200, minusX=iconX+240, btnY=iconY+8;
        if(mx>=plusX && mx<=plusX+btnW && my>=btnY && my<=btnY+btnH) { knightCount++; }
        if(mx>=minusX && mx<=minusX+btnW && my>=btnY && my<=btnY+btnH && knightCount>0) { knightCount--; }
        // Gunner + -
        iconY += 80;
        float plusX2=iconX+200, minusX2=iconX+240, btnY2=iconY+8;
        if(mx>=plusX2 && mx<=plusX2+btnW && my>=btnY2 && my<=btnY2+btnH) { gunnerCount++; }
        if(mx>=minusX2 && mx<=minusX2+btnW && my>=btnY2 && my<=btnY2+btnH && gunnerCount>0) { gunnerCount--; }
        // maxUnit + -
        iconY += 80;
        float plusX3=iconX+200, minusX3=iconX+240, btnY3=iconY+8;
        if(mx>=plusX3 && mx<=plusX3+btnW && my>=btnY3 && my<=btnY3+btnH) { maxUnit++; }
        if(mx>=minusX3 && mx<=minusX3+btnW && my>=btnY3 && my<=btnY3+btnH && maxUnit>1) { maxUnit--; }
    }

    float thirdTabWidth = toolbarSize / 3;
    if (my >= 0 && my <= tabHeight) {
        if (mx >= tabStartX && mx < tabStartX + thirdTabWidth) {
            currentSection = SECTION_MAP;
            removeButtonSelected = false;
            return;
        } else if (mx >= tabStartX + thirdTabWidth && mx < tabStartX + 2 * thirdTabWidth) {
            currentSection = SECTION_UNIT;
            removeButtonSelected = false;
            return;
        } else if (mx >= tabStartX + 2 * thirdTabWidth && mx < tabEndX) {
            currentSection = SECTION_PLAYER;
            removeButtonSelected = false;
            return;
        }
    }

    float btnW = 120, btnH = 48;
    float screenH = Engine::GameEngine::GetInstance().GetScreenSize().y;
    float btnX = screenW - btnW - 20;
    float btnY = screenH - btnH - 20;

    float saveBtnW = 120, saveBtnH = 48;
    float saveBtnX = screenW - saveBtnW - 20;
    float saveBtnY = screenH - saveBtnH - 20;
    
    if (button == 1) {
        // Check if Save button is clicked
        if (mx >= saveBtnX && mx <= saveBtnX + saveBtnW && my >= saveBtnY && my <= saveBtnY + saveBtnH) {
            SaveMapAndUnits();
            std::cout << "[DEBUG] Map and units saved (button)." << std::endl;
            return;
        }
        // Cancel button
        float cancelBtnW = 120, cancelBtnH = 48;
        float cancelBtnX = saveBtnX - cancelBtnW - 20;
        float cancelBtnY = saveBtnY;
        if (mx >= cancelBtnX && mx <= cancelBtnX + cancelBtnW && my >= cancelBtnY && my <= cancelBtnY + cancelBtnH) {
            Engine::GameEngine::GetInstance().ChangeScene("start");
            std::cout << "[DEBUG] Cancel button pressed, returning to start scene." << std::endl;
            return;
        }
        placing = 1; 
        // Enable continuous placement/removal while mouse is held
        // Remove legacy bottom-right remove button logic
        // Only handle placing/removing on the map area
        if (mx < screenW - toolbarSize && my > tabHeight) {
            float worldX = mx + cameraX;
            float worldY = my + cameraY;
            int gridX = static_cast<int>(worldX) / BlockSize;
            int gridY = static_cast<int>(worldY) / BlockSize;
            if (currentSection == SECTION_MAP) {
                if (removeButtonSelected) {
                    RemoveAtGrid(gridX, gridY);
                } else if (selectedTileIndex >= 0 && selectedTileIndex < static_cast<int>(tileTypes.size())) {
                    if (gridX >= 0 && gridX < MapWidth && gridY >= 0 && gridY < MapHeight) {
                        if (mapState[gridY][gridX] != selectedTileIndex) {
                            mapState[gridY][gridX] = selectedTileIndex;
                            lastPlacedGridX = gridX;
                            lastPlacedGridY = gridY;
                        }
                    }
                }
            } else if (currentSection == SECTION_UNIT) {
                if (removeButtonSelected) {
                    RemoveAtGrid(gridX, gridY);
                } else if (selectedEnemyIndex >= 0 && selectedEnemyIndex < static_cast<int>(enemyTypes.size())) {
                    if (gridX > 0 && gridX < MapWidth && gridY > 0 && gridY < MapHeight) {
                        auto it = std::remove_if(unitData.begin(), unitData.end(), [gridX, gridY](const std::tuple<int, float, float>& unit) {
                            int ux = static_cast<int>(std::get<1>(unit)) - 1;
                            int uy = static_cast<int>(std::get<2>(unit)) - 1;
                            return ux == gridX && uy == gridY;
                        });
                        if (it != unitData.end()) unitData.erase(it, unitData.end());
                        int type = enemyTypes[selectedEnemyIndex];
                        unitData.emplace_back(type, gridX + 1, gridY + 1);
                        lastPlacedGridX = gridX;
                        lastPlacedGridY = gridY;
                    }
                }
            } else if(currentSection==SECTION_PLAYER){

            }
        }
    }
    if (button == 2) {
        dragging = true;
        dragStartX = mx;
        dragStartY = my;
        cameraStartX = cameraX;
        cameraStartY = cameraY;
    }
}

void MapEditorScene::OnMouseMove(int mx, int my) {
    IScene::OnMouseMove(mx, my);

    if (dragging) {
        int dx = mx - dragStartX;
        int dy = my - dragStartY;
        cameraX = cameraStartX - dx;
        cameraY = cameraStartY - dy;
    }
    float screenW = Engine::GameEngine::GetInstance().GetScreenSize().x;
    float screenH = Engine::GameEngine::GetInstance().GetScreenSize().y;  
    float toolbarSize = screenW * 0.4f;
    float tabHeight = 40;    
    
    /*The button's attributes  */
    float btnW = 120, btnH = 48;
    float btnX = screenW - btnW - 20;
    float btnY = screenH - btnH - 20;

    // --- Continuous placement/removal when placing == 1 ---
    if (placing == 1 && mx < screenW - toolbarSize && my > tabHeight) {
        float worldX = mx + cameraX;
        float worldY = my + cameraY;
        int gridX = static_cast<int>(worldX) / BlockSize;
        int gridY = static_cast<int>(worldY) / BlockSize;
        if (gridX != lastPlacedGridX || gridY != lastPlacedGridY) {
            if (currentSection == SECTION_MAP) {
                if (removeButtonSelected) {
                    RemoveAtGrid(gridX, gridY);
                } else if (selectedTileIndex >= 0 && selectedTileIndex < static_cast<int>(tileTypes.size())) {
                    if (gridX >= 0 && gridX < MapWidth && gridY >= 0 && gridY < MapHeight) {
                        if (mapState[gridY][gridX] != selectedTileIndex) {
                            mapState[gridY][gridX] = selectedTileIndex;
                            lastPlacedGridX = gridX;
                            lastPlacedGridY = gridY;
                        }
                    }
                }
            } else if (currentSection == SECTION_UNIT) {
                if (removeButtonSelected) {
                    RemoveAtGrid(gridX, gridY);
                } else if (selectedEnemyIndex >= 0 && selectedEnemyIndex < static_cast<int>(enemyTypes.size())) {
                    if (gridX > 0 && gridX < MapWidth && gridY > 0 && gridY < MapHeight) {
                        auto it = std::remove_if(unitData.begin(), unitData.end(), [gridX, gridY](const std::tuple<int, float, float>& unit) {
                            int ux = static_cast<int>(std::get<1>(unit)) - 1;
                            int uy = static_cast<int>(std::get<2>(unit)) - 1;
                            return ux == gridX && uy == gridY;
                        });
                        if (it != unitData.end()) unitData.erase(it, unitData.end());
                        int type = enemyTypes[selectedEnemyIndex];
                        unitData.emplace_back(type, gridX + 1, gridY + 1);
                        lastPlacedGridX = gridX;
                        lastPlacedGridY = gridY;
                    }
                }
            }
        }
    }

    float reservedWidth = screenW * 0.5f; /*右邊要讓嚟放 Map Assets*/
    float reservedSpace = screenW * 0.1f;
    float maxCameraX = std::max(0.0f - reservedSpace, static_cast<float>(MapWidth * BlockSize) - (screenW - reservedWidth));
    float maxCameraY = std::max(0.0f - reservedSpace, static_cast<float>(MapHeight * BlockSize) - (screenH - reservedSpace));
    cameraX = std::max(0.0f - reservedSpace, std::min(cameraX, maxCameraX));
    cameraY = std::max(0.0f - reservedSpace, std::min(cameraY, maxCameraY));
}

void MapEditorScene::OnMouseUp(int button, int mx, int my) {
    IScene::OnMouseUp(button, mx, my);

    if (button == 1) {
        dragging = false;
        placing = false; // Stop continuous placement/removal
        lastPlacedGridX = -1; lastPlacedGridY = -1;
    }

    if (button == 2) {
        dragging = false;
    }
}

void MapEditorScene::SaveMapAndUnits() {
    // Save mapState to Resource/mapX.txt
    std::string mapFilename = "Resource/map" + std::to_string(MapId) + ".txt";
    std::ofstream mapOut(mapFilename, std::ios::out | std::ios::trunc);
    if (mapOut.is_open()) {
        mapOut << mapState[0].size() << " " << mapState.size()<<" "; // Save width and height
        mapOut<< knightCount <<" "<<gunnerCount<<" "<<maxUnit<<"\n";
        for (size_t y = 0; y < mapState.size(); ++y) {
            for (size_t x = 0; x < mapState[y].size(); ++x) {
                mapOut << mapState[y][x];
            }
            mapOut << "\n";
        }
        mapOut.close();
    }
    // Save unitData to Resource/enemyX.txt
    std::string enemyFilename = "Resource/enemy" + std::to_string(MapId) + ".txt";
    std::ofstream enemyOut(enemyFilename, std::ios::out | std::ios::trunc);
    if (enemyOut.is_open()) {
        for (const auto& unit : unitData) {
            int type = std::get<0>(unit);
            float ux = std::get<1>(unit);
            float uy = std::get<2>(unit);
            enemyOut << type << " " << ux << " " << uy << "\n";
        }
        enemyOut.close();
    }
    Engine::GameEngine::GetInstance().ChangeScene("start");
}

void MapEditorScene::OnKeyDown(int keyCode) {
    // S or s for save
    if (keyCode == ALLEGRO_KEY_S) {
        SaveMapAndUnits();
        std::cout << "[DEBUG] Map and units saved." << std::endl;
    }
    // ...existing code...
}

// --- Remove Button State ---
bool MapEditorScene::removeButtonSelected = false;

void MapEditorScene::RemoveAtGrid(int gridX, int gridY) {
    if (currentSection == SECTION_MAP) {
        if (gridX >= 0 && gridX < MapWidth && gridY >= 0 && gridY < MapHeight) {
            if (mapState[gridY][gridX] != 0) {
                mapState[gridY][gridX] = 0;
            }
        }
    } else if (currentSection == SECTION_UNIT) {
        if (gridX >= 0 && gridX < MapWidth && gridY >= 0 && gridY < MapHeight) {
            auto it = std::remove_if(unitData.begin(), unitData.end(), [gridX, gridY](const std::tuple<int, float, float>& unit) {
                int ux = static_cast<int>(std::get<1>(unit)) - 1;
                int uy = static_cast<int>(std::get<2>(unit)) - 1;
                return ux == gridX && uy == gridY;
            });
            if (it != unitData.end()) unitData.erase(it, unitData.end());
        }
    }
}