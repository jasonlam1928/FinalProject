//
// Created by Hsuan on 2024/4/10.
//

#include "StartScene.h"
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_ttf.h>
#include <functional>
#include <memory>
#include <string>

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "PlayScene.hpp"
#include "Scene/StartScene.h"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Component/Slider.hpp"

// TODO HACKATHON-2 (1/3): You can imitate the 2 files: 'StartScene.hpp', 'StartScene.cpp' to implement your SettingsScene.
void StartScene::Initialize() {
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;
    Engine::ImageButton *btn;

    AddNewObject(new Engine::Label("Tower Defense", "pirulen.ttf", 120, halfW, halfH / 3 + 50, 10, 255, 255, 255, 0.5, 0.5));

    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 200, halfH / 2 + 200, 400, 100);
    btn->SetOnClickCallback(std::bind(&StartScene::PlayOnClick, this, 1));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Play", "pirulen.ttf", 48, halfW, halfH / 2 + 250, 0, 0, 0, 255, 0.5, 0.5));

    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 200, halfH * 3 / 2 - 50, 400, 100);
    btn->SetOnClickCallback(std::bind(&StartScene::SettingsOnClick, this, 2));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Settings", "pirulen.ttf", 48, halfW, halfH * 3 / 2, 0, 0, 0, 255, 0.5, 0.5));
}
void StartScene::Terminate() {
    IScene::Terminate();
}
void StartScene::PlayOnClick(int stage) {
    Engine::GameEngine::GetInstance().ChangeScene((!debugMode) ? "stage-select" : "map-editor-select");
}
void StartScene::SettingsOnClick(int stage) {
    Engine::GameEngine::GetInstance().ChangeScene("Settings");
}

void StartScene::OnKeyDown(int keyCode) {
    IScene::OnKeyDown(keyCode);
    /*如果 Shift+F7 就 Debug Mode*/
    if (keyCode == ALLEGRO_KEY_F7) {
        debugMode = !debugMode;
        debugModeMessageTimer = 2.0f; // Show message for 2 seconds
    }
}

void StartScene::Draw() const {
    IScene::Draw();
    if (debugModeMessageTimer > 0.0f) {
        float alpha = std::min(1.0f, debugModeMessageTimer / 2.0f);
        int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
        ALLEGRO_COLOR color = al_map_rgba_f(1, 1, 1, alpha); /*白色嘅*/
        al_draw_text(
            Engine::Resources::GetInstance().GetFont("pirulen.ttf", 48).get(),
            color, w / 2, 40, ALLEGRO_ALIGN_CENTRE, (debugMode) ? "Debug Mode Enabled" : "Debug Mode Disabled"
            );
    }
}

void StartScene::Update(float deltaTime) {
    IScene::Update(deltaTime);
    if (debugModeMessageTimer > 0.0f) {
        debugModeMessageTimer -= deltaTime;
        if (debugModeMessageTimer <= 0.0f) {
            debugModeMessageTimer = 0.0f;
        }
    }
}