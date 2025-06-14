#include <allegro5/allegro_audio.h>
#include <functional>
#include <memory>
#include <string>
#include <iostream>

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "MapEditorScene.hpp"
#include "PlayScene.hpp"
#include "MapEditorSelectScene.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Component/Slider.hpp"

void MapEditorSelectScene::Initialize() {
    std::cout << "[DEBUG] MapEditorSelectScene::Initialize called" << std::endl;
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;
    Engine::ImageButton *btn;

    AddNewObject(new Engine::Label("Map Editor", "pirulen.ttf", 48, halfW, halfH * 0.15, 255, 255, 255, 255, 0.5, 0.5));

    btn = new Engine::ImageButton("stage-select/ButtonIdle.png", "stage-select/ButtonHover.png", halfW - 200, halfH / 2 - 50, 400, 100);
    btn->SetOnClickCallback(std::bind(&MapEditorSelectScene::PlayOnClick, this, 1));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Stage 1", "pirulen.ttf", 48, halfW, halfH / 2, 0, 0, 0, 255, 0.5, 0.5));
    btn = new Engine::ImageButton("stage-select/ButtonIdle.png", "stage-select/ButtonHover.png", halfW - 200, halfH / 2 + 100, 400, 100);
    btn->SetOnClickCallback(std::bind(&MapEditorSelectScene::PlayOnClick, this, 2));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Stage 2", "pirulen.ttf", 48, halfW, halfH / 2 + 150, 0, 0, 0, 255, 0.5, 0.5));

    btn = new Engine::ImageButton("stage-select/ButtonIdle.png", "stage-select/ButtonHover.png", halfW - 200, halfH * 3 / 2 - 50, 400, 100);
    btn->SetOnClickCallback(std::bind(&MapEditorSelectScene::BackOnClick, this, 1));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Back", "pirulen.ttf", 48, halfW, halfH * 3 / 2, 0, 0, 0, 255, 0.5, 0.5));
}
void MapEditorSelectScene::Terminate() {
    IScene::Terminate();
}
void MapEditorSelectScene::BackOnClick(int stage) {
    Engine::GameEngine::GetInstance().ChangeScene("start");
}
void MapEditorSelectScene::PlayOnClick(int stage) {
    std::cout << "[DEBUG] PlayOnClick called with stage: " << stage << std::endl;
    MapEditorScene *scene = dynamic_cast<MapEditorScene *>(Engine::GameEngine::GetInstance().GetScene("map-editor"));
    scene->MapId = stage;
    std::cout << "[DEBUG ]Changing to map-editor with MapId: " << stage << "..." << std::endl;
    Engine::GameEngine::GetInstance().ChangeScene("map-editor");
}
void MapEditorSelectScene::ScoreboardOnClick(int stage) {
    Engine::GameEngine::GetInstance().ChangeScene("Scoreboard");
}
void MapEditorSelectScene::Draw() const {
    IScene::Draw();
    // You can add custom drawing code here if needed
}