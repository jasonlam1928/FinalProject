#include "MapEditorScene.hpp"
#include <fstream>
#include <allegro5/allegro_primitives.h>
#include <iostream>

const int MapWidth = 64, MapHeight = 64, BlockSize = 64;

void MapEditorScene::Initialize() {
    mapState = std::vector<std::vector<TileType>>(MapHeight, std::vector<TileType>(MapWidth, TILE_DIRT));
}

void MapEditorScene::Draw() const {
    al_clear_to_color(al_map_rgb(50, 50, 50));
    for (int y = 0; y < MapHeight; ++y) {
        for (int x = 0; x < MapWidth; ++x) {
            ALLEGRO_COLOR color = (mapState[y][x] == TILE_FLOOR) ? al_map_rgb(200, 200, 200) : al_map_rgb(120, 80, 40);
            al_draw_filled_rectangle(
                x * BlockSize - cameraX, y * BlockSize - cameraY,
                (x + 1) * BlockSize - cameraX, (y + 1) * BlockSize - cameraY,
                color
            );
            al_draw_rectangle(
                x * BlockSize - cameraX, y * BlockSize - cameraY,
                (x + 1) * BlockSize - cameraX, (y + 1) * BlockSize - cameraY,
                al_map_rgb(0,0,0), 1
            );
        }
    }
}

void MapEditorScene::OnMouseDown(int button, int mx, int my) {
    if (button == 1) {
        int x = (mx + cameraX) / BlockSize;
        int y = (my + cameraY) / BlockSize;
        if (x >= 0 && x < MapWidth && y >= 0 && y < MapHeight) {
            mapState[y][x] = static_cast<TileType>(selectedTile);
        }
    }
    if (button == 2) { // right mouse for camera drag
        dragging = true;
        dragStartX = mx;
        dragStartY = my;
        cameraStartX = cameraX;
        cameraStartY = cameraY;
    }
}

void MapEditorScene::OnMouseMove(int mx, int my) {
    if (dragging) {
        cameraX = cameraStartX - (mx - dragStartX);
        cameraY = cameraStartY - (my - dragStartY);
    }
}

void MapEditorScene::OnMouseUp(int button, int mx, int my) {
    if (button == 2) dragging = false;
}

void MapEditorScene::OnKeyDown(int keyCode) {
    if (keyCode == ALLEGRO_KEY_1) selectedTile = TILE_DIRT;
    if (keyCode == ALLEGRO_KEY_2) selectedTile = TILE_FLOOR;
    if (keyCode == ALLEGRO_KEY_S) SaveMap();
}

void MapEditorScene::SaveMap() {
    std::ofstream fout("Resource/map1.txt");
    for (int y = 0; y < MapHeight; ++y)
        for (int x = 0; x < MapWidth; ++x)
            fout << (mapState[y][x] == TILE_FLOOR ? '1' : '0');
    fout.close();
    std::cout << "Map saved!" << std::endl;
}