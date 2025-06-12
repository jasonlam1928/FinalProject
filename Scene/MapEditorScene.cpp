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
    AddNewObject(EnemyGroup = new Engine::Group());

    // Initialize tileTypes before using it
    tileTypes = {
        "play/dirt.png",   // 0
        "play/floor.png", // 1
        "play/floor.png"  // 2 (or another image if you have more types)
    };

    // Load map (reuse PlayScene logic)
    std::string filename = std::string("Resource/map") + std::to_string(MapId) + ".txt";
    std::cout << "{Initialising MapEditor} Loading map from: " << filename << std::endl;
    char c;

    // Read tile data into a vector<int>
    std::vector<int> tileData;
    std::ifstream fin(filename);
    while (fin >> c) {
        switch (c) {
            case '0': tileData.push_back(0); break;
            case '1': tileData.push_back(1); break;
            case '2': tileData.push_back(2); break;
            case '\n':
            case '\r':
                break;
            default: throw std::ios_base::failure("Map data is corrupted.");
        }
    }
    fin.close();

    if (tileData.size() != MapWidth * MapHeight) {
        throw std::runtime_error("Map data size does not match expected dimensions.");
    }

    mapState = std::vector<std::vector<int>>(MapHeight, std::vector<int>(MapWidth));
    for (int i = 0; i < MapHeight; i++) {
        for (int j = 0; j < MapWidth; j++) {
            int num = tileData[i * MapWidth + j];
            mapState[i][j] = num;
            if (num >= 0 && num < static_cast<int>(tileTypes.size()))
                TileMapGroup->AddNewObject(new Engine::Image(tileTypes[num], j * BlockSize, i * BlockSize, BlockSize, BlockSize));
            else
                TileMapGroup->AddNewObject(new Engine::Image("play/dirt.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
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
        float posX = (ux - 1) * BlockSize + BlockSize / 2;
        float posY = (uy - 1) * BlockSize + BlockSize / 2;
        Engine::Sprite* enemy = nullptr;
        switch (typeInt) {
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
        switch (type) {
            case 1:
                spritePath = "play/Knight_01.png";
                break;
            case 2:
                spritePath = "play/Knight_03.png";
                break;
            default:
                spritePath = "play/Knight_01.png";
        }
        ALLEGRO_BITMAP* bmp = Engine::Resources::GetInstance().GetBitmap(spritePath).get();
        if (bmp) {
            float bmpW = al_get_bitmap_width(bmp);
            float bmpH = al_get_bitmap_height(bmp);
            float drawW = bmpW / 2.0f;
            float drawH = bmpH / 2.0f;
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
    al_draw_filled_rectangle(screenW - toolbarSize, 0, screenW - toolbarSize + tabWidth, tabHeight, mapTabColor);
    al_draw_filled_rectangle(screenW - toolbarSize + tabWidth, 0, screenW, tabHeight, enemyTabColor);
    al_draw_text(al_create_builtin_font(), al_map_rgb(255,255,255), screenW - toolbarSize + tabWidth/2, 10, ALLEGRO_ALIGN_CENTRE, "Map");
    al_draw_text(al_create_builtin_font(), al_map_rgb(255,255,255), screenW - toolbarSize + tabWidth + tabWidth/2, 10, ALLEGRO_ALIGN_CENTRE, "Enemy");
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
                    spritePath = "play/Knight_01.png";
                    baseColor = al_map_rgb(255, 0, 0);
                    break;
                case 2:
                    spritePath = "play/Knight_03.png";
                    baseColor = al_map_rgb(0, 255, 0);
                    break;
                default:
                    spritePath = "play/Knight_01.png";
                    baseColor = al_map_rgb(255, 0, 0);
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
    // Draw Save button at the bottom of the toolbar
    float saveBtnW = 120, saveBtnH = 48;
    float saveBtnX = screenW - saveBtnW - 20;
    float saveBtnY = screenH - saveBtnH - 20;
    al_draw_filled_rectangle(saveBtnX, saveBtnY, saveBtnX + saveBtnW, saveBtnY + saveBtnH, al_map_rgb(60, 180, 60));
    al_draw_rectangle(saveBtnX, saveBtnY, saveBtnX + saveBtnW, saveBtnY + saveBtnH, al_map_rgb(255,255,255), 2);
    al_draw_text(al_create_builtin_font(), al_map_rgb(255,255,255), saveBtnX + saveBtnW/2, saveBtnY + saveBtnH/2 - 10, ALLEGRO_ALIGN_CENTRE, "Save");
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

    if (my >= 0 && my <= tabHeight) {
        if (mx >= tabStartX && mx < tabStartX + tabWidth) {
            currentSection = SECTION_MAP;
            removeButtonSelected = false;
            return;
        } else if (mx >= tabStartX + tabWidth && mx < tabEndX) {
            currentSection = SECTION_UNIT;
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
                    if (gridX >= 0 && gridX < MapWidth && gridY >= 0 && gridY < MapHeight) {
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
                    if (gridX >= 0 && gridX < MapWidth && gridY >= 0 && gridY < MapHeight) {
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
        for (int y = 0; y < MapHeight; ++y) {
            for (int x = 0; x < MapWidth; ++x) {
                mapOut << mapState[y][x];
            }
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