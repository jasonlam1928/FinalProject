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
#include "Unit/Enemy/Enemy1.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Group.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Component/ImageButton.hpp"
#include "MapEditorScene.hpp"

#include "Unit/Turret/Turret.hpp"
#include "Unit/Turret/Turret1.hpp"

const int MapWidth = 64, MapHeight = 64;
const int BlockSize = 64;

void MapEditorScene::Initialize() {
    std::cout << "[DEBUG] MapEditorScene::Initialize called" << std::endl;
    cameraX = cameraY = 0;
    dragging = false;
    AddNewObject(TileMapGroup = new Engine::Group());

    // Load map (reuse PlayScene logic)
    std::string filename = std::string("Resource/map") + std::to_string(MapId) + ".txt";
    std::cout << "{Initialising MapEditor} Loading map from: " << filename << std::endl;
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

    if (mapData.size() != MapWidth * MapHeight) {
    throw std::runtime_error("Map data size does not match expected dimensions.");
    }

    std::cout << "{Initialising MapEditor} Tiles loaded: " << TileMapGroup->GetObjects().size() << std::endl;

    mapState = std::vector<std::vector<int>>(MapHeight, std::vector<int>(MapWidth));
    for (int i = 0; i < MapHeight; i++) {
        for (int j = 0; j < MapWidth; j++) {
            const int num = mapData[i * MapWidth + j];
            mapState[i][j] = num;
            if (num)
                TileMapGroup->AddNewObject(new Engine::Image("play/floor.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
            else
                TileMapGroup->AddNewObject(new Engine::Image("play/dirt.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
        }
    }


    std::cout << "{Initialising MapEditor} Enemy group loading..." << std::endl;
    AddNewObject(EnemyGroup = new Engine::Group());

    std::string enemyFilename = std::string("Resource/enemy") + std::to_string(MapId) + ".txt";
    std::ifstream enemyFin(enemyFilename);
    float type, x, y;
    while (enemyFin >> type && enemyFin >> x && enemyFin >> y) {
        float posX = (x - 1) * BlockSize + BlockSize / 2;
        float posY = (y - 1) * BlockSize + BlockSize / 2;
        Engine::Sprite* enemy = nullptr;
        switch (static_cast<int>(type)) {
            case 1:
                enemy = new Knight(posX, posY);
                break;
            case 2:
                enemy = new Knight1(posX, posY);
                break;
        }
        if (enemy) {
            EnemyGroup->AddNewObject(enemy);
        }
    }
    enemyFin.close();

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
    IScene::Draw();
    al_identity_transform(&Camera);
    al_use_transform(&Camera);

    float screenW = Engine::GameEngine::GetInstance().GetScreenSize().x;
    float screenH = Engine::GameEngine::GetInstance().GetScreenSize().y;
    float toolbarSize = screenW * 0.4f; 

    al_draw_filled_rectangle(screenW - toolbarSize, 0, screenW, screenH, al_map_rgb(40, 40, 40)); /*暫時嘅背景*/

    // Toolbar background
    al_draw_filled_rectangle(screenW - toolbarSize, 0, screenW, screenH, al_map_rgb(40, 40, 40));

    // Tab buttons
    float tabHeight = 40;
    float tabWidth = toolbarSize / 2;
    ALLEGRO_COLOR mapTabColor = (currentSection == SECTION_MAP) ? al_map_rgb(80, 120, 200) : al_map_rgb(80, 80, 80);
    ALLEGRO_COLOR enemyTabColor = (currentSection == SECTION_UNIT) ? al_map_rgb(80, 120, 200) : al_map_rgb(80, 80, 80);

    al_draw_filled_rectangle(screenW - toolbarSize, 0, screenW - toolbarSize + tabWidth, tabHeight, mapTabColor);
    al_draw_filled_rectangle(screenW - toolbarSize + tabWidth, 0, screenW, tabHeight, enemyTabColor);

    al_draw_text(al_create_builtin_font(), al_map_rgb(255,255,255), screenW - toolbarSize + tabWidth/2, 10, ALLEGRO_ALIGN_CENTRE, "Map");
    al_draw_text(al_create_builtin_font(), al_map_rgb(255,255,255), screenW - toolbarSize + tabWidth + tabWidth/2, 10, ALLEGRO_ALIGN_CENTRE, "Enemy");

    // Section content
    if (currentSection == SECTION_MAP) {
        // Draw map tools/content here
        al_draw_text(al_create_builtin_font(), al_map_rgb(255,255,255), screenW - toolbarSize + 20, tabHeight + 20, 0, "Map Section Content");
    } else if (currentSection == SECTION_UNIT) {
        // Draw enemy tools/content here
        al_draw_text(al_create_builtin_font(), al_map_rgb(255,255,255), screenW - toolbarSize + 20, tabHeight + 20, 0, "Enemy Section Content");
    }

    if (currentSection == SECTION_MAP) {
        // Draw tile selection
        for (size_t i = 0; i < tileTypes.size(); ++i) {
            float iconX = screenW - toolbarSize + 20 + i * (BlockSize + 10);
            float iconY = tabHeight + 40;
            al_draw_bitmap(Engine::Resources::GetInstance().GetBitmap(tileTypes[i]).get(), iconX, iconY, 0);
            if (i == selectedTileIndex) {
                al_draw_rectangle(iconX, iconY, iconX + BlockSize, iconY + BlockSize, al_map_rgb(255, 255, 0), 3);
            }
        }
    } else if (currentSection == SECTION_UNIT) {
    // Draw enemy selection (sprite on colored base)
        for (size_t i = 0; i < enemyTypes.size(); ++i) {
            float iconX = screenW - toolbarSize + 20 + i * (BlockSize + 10);
            float iconY = tabHeight + 40;

            // Determine base color (red for enemy, green for player)
            ALLEGRO_COLOR baseColor;
            // Example: all are enemies, so red. If you have player units, add logic here.
            baseColor = al_map_rgb(255, 0, 0);

            // Draw base rectangle
            al_draw_filled_rectangle(iconX, iconY, iconX + BlockSize, iconY + BlockSize, baseColor);

            // Select sprite path
            std::string spritePath;
            switch (enemyTypes[i]) {
                case 1:
                    spritePath = "play/Knight_01.png";
                    break;
                case 2:
                    spritePath = "play/Knight_03.png";
                    break;
                // Add more cases as needed
                default:
                    spritePath = "play/Knight_01.png";
            }

            // Draw sprite on top of base
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

            // Draw selection rectangle if selected
            if (i == selectedEnemyIndex) {
                al_draw_rectangle(iconX, iconY, iconX + BlockSize, iconY + BlockSize, al_map_rgb(255, 255, 0), 3);
            }
        }
    }
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
                selectedTileIndex = i;
                return;
            }
        }
    } else if (currentSection == SECTION_UNIT) {
        for (size_t i = 0; i < enemyTypes.size(); ++i) {
            float iconX = screenW - toolbarSize + 20 + i * (BlockSize + 10);
            if (mx >= iconX && mx <= iconX + BlockSize && my >= iconY && my <= iconY + BlockSize) {
                selectedEnemyIndex = i;
                return;
            }
        }
    }

    // Check if click is within the tab area
    if (my >= 0 && my <= tabHeight) {
        if (mx >= tabStartX && mx < tabStartX + tabWidth) {
            // Clicked on Map tab
            currentSection = SECTION_MAP;
            return;
        } else if (mx >= tabStartX + tabWidth && mx < tabEndX) {
            // Clicked on Enemy tab
            currentSection = SECTION_UNIT;
            return;
        }
    }

    if (button == 1) {
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
    }
}