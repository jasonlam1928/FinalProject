#include <allegro5/allegro_audio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <functional>
#include <memory>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <ctime>

#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Group.hpp"
#include "Engine/LOG.hpp"

#include "Scoreboard.hpp"
#include "StageSelectScene.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Component/Slider.hpp"

using namespace std;

int counts=0;
int cur=0;
vector<Engine::Label*> scoreLabels;
vector<PlayerRecord> playersRecord;
void Scoreboard::UpdateScorePage() {
    for (auto lbl : scoreLabels) {
        RemoveObjectByInstance(lbl); // 如果有這個函數
        delete lbl;
    }
    scoreLabels.clear();

    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int halfW = w / 2;

    for (int i = cur; i < cur + 5 && i < counts; ++i) {
        string rank = to_string(i + 1);
        auto* label = new Engine::Label(rank + " " + playersRecord[i].name + " " + "stage:"+ to_string(playersRecord[i].stage) + " " + to_string(playersRecord[i].score) + "   "+ playersRecord[i].time,
                                        "pirulen.ttf", 24, halfW, 150 + 80 * (i - cur),
                                        255, 255, 255, 255, 0.5, 0.5);
        AddNewObject(label);
        scoreLabels.push_back(label);
    }
}
void Scoreboard::Initialize() {
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;
    Engine::ImageButton *btn;
    counts=0;
    cur = 0;

    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 200, halfH * 3 / 2 - 50, 400, 100);
    btn->SetOnClickCallback(std::bind(&Scoreboard::BackOnClick, this, 1));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Back", "pirulen.ttf", 48, halfW, halfH * 3 / 2, 0, 0, 0, 255, 0.5, 0.5));

    AddNewObject(new Engine::Label("ScoreBoard", "pirulen.ttf", 48, halfW, 50, 0, 255, 0, 255, 0.5, 0.5));

    ifstream file("Resource/scoreboard.txt");
    string name;
    string time;
    int score;
    int stage;
    
    if(file.is_open()){
        cout<<"Load";
        while(file>>name>>stage>>score>>time){
            counts++;
            PlayerRecord p;
            p.name = name;
            p.stage = stage;
            p.score = score;
            p.time = time;
            playersRecord.push_back(p);
        }
        file.close();
    }
    else{
        cout<<"wrong";
    }
    UpdateScorePage();

    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 700, halfH * 3 / 2 - 50, 400, 100);
    btn->SetOnClickCallback(std::bind(&Scoreboard::PrevPage, this, 1));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Prev", "pirulen.ttf", 48, halfW-500, halfH * 3 / 2, 0, 0, 0, 255, 0.5, 0.5));

    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW + 300, halfH * 3 / 2 - 50, 400, 100);
    btn->SetOnClickCallback(std::bind(&Scoreboard::NextPage, this, 1));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Next", "pirulen.ttf", 48, halfW+500, halfH * 3 / 2, 0, 0, 0, 255, 0.5, 0.5));
}

void Scoreboard::Terminate() {
    for (auto lbl : scoreLabels) {
        RemoveObjectByInstance(lbl);
        delete lbl;
    }
    scoreLabels.clear();

    // 清空玩家記錄
    playersRecord.clear();
    counts = 0;
    cur = 0;
    IScene::Terminate();
}

void Scoreboard::BackOnClick(int stage) {
    Engine::GameEngine::GetInstance().ChangeScene("stage-select");
}

void Scoreboard::NextPage(int type){
    if(cur+5<counts){
        cur+=5;
        UpdateScorePage();
    }
    
}

void Scoreboard::PrevPage(int type){
    if(cur-5>=0){
        cur-=5;
        UpdateScorePage();
    }
}

vector<PlayerRecord> getPlayerRecord(){
    return playersRecord;
}