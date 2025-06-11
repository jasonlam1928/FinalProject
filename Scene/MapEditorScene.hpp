#ifndef MAPEDITORSCENE_HPP
#define MAPEDITORSCENE_HPP

#include "Engine/IScene.hpp"
#include "Engine/Point.hpp"
#include <vector>

enum ToolbarSection {
    SECTION_MAP,
    SECTION_UNIT,
};

class MapEditorScene final : public Engine::IScene {
private:
    ToolbarSection currentSection = SECTION_MAP;
    Engine::Group* TileMapGroup;
    Engine::Group* EnemyGroup;
    std::vector<std::vector<int>> mapState;
    float cameraX = 0, cameraY = 0;
    bool dragging = false;
    int dragStartX = 0, dragStartY = 0;
    float cameraStartX = 0, cameraStartY = 0;
    int selectedTileIndex = 0;
    int selectedEnemyIndex = 0;
    std::vector<std::string> tileTypes = {"play/floor.png", "play/dirt.png"}; /*Add your tile image paths*/
    std::vector<int> enemyTypes = {1, 2};

public:    
    int MapId = 1;
    explicit MapEditorScene() = default;
    void Initialize() override;
    void Terminate() override;
    void Update(float deltaTime) override;
    void Draw() const override;
    void OnMouseMove(int mx, int my) override;
    void OnMouseUp(int button, int mx, int my) override;
    void OnMouseDown(int button, int mx, int my) override;
};

#endif   // MAPEDITORSCENE_HPP