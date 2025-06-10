#ifndef MAPEDITORSCENE_HPP
#define MAPEDITORSCENE_HPP

#include "Engine/IScene.hpp"
#include "Engine/Point.hpp"
#include <vector>

class MapEditorScene final : public Engine::IScene {
private:
    enum TileType {
        TILE_DIRT,
        TILE_FLOOR,
    };
    std::vector<std::vector<TileType>> mapState;
    int selectedTile = TILE_FLOOR;
    float cameraX = 0, cameraY = 0;
    bool dragging = false;
    int dragStartX = 0, dragStartY = 0;
    float cameraStartX = 0, cameraStartY = 0;
public:
    void Initialize() override;
    void Draw() const override;
    void OnMouseDown(int button, int mx, int my) override;
    void OnMouseMove(int mx, int my) override;
    void OnMouseUp(int button, int mx, int my) override;
    void OnKeyDown(int keyCode) override;
    void SaveMap();
};

#endif   // MAPEDITORSCENE_HPP