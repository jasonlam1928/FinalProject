#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_ttf.h>
#include <functional>
#include <algorithm>
#include <string>
#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include "Save.hpp"

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "Engine/Group.hpp"
#include "Engine/LOG.hpp"
#include "Engine/Resources.hpp"
#include "LoseScene.hpp"
#include "PlayScene.hpp"
#include "Scoreboard.hpp"
#include "UI/Component/Image.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
using namespace std;

bool comparePlayers(const PlayerRecord& a, const PlayerRecord& b) {
    if (a.score != b.score) {
        return a.score > b.score; // 降序排列
    }
    return a.name < b.name;
}

int Namelen;
int Maxlen=20;
ALLEGRO_FONT* font40;
void Save::Initialize() {
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;

    Engine::ImageButton *btn;
    AddNewObject(new Engine::Label("Enter your name: ", "pirulen.ttf", 48, halfW-200, halfH / 4 + 10, 255, 255, 255, 255, 0.5, 0.5));
    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 200, halfH * 3 / 2 - 50, 400, 100);
    btn->SetOnClickCallback(std::bind(&Save::SaveRecord, this, 1));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Save", "pirulen.ttf", 48, halfW, halfH * 3 / 2, 0, 0, 0, 255, 0.5, 0.5));
    Namelen = 0;

}
char PlayerName[20];
Engine::Label* name;
void Save::Terminate() {
    if (name) {
        RemoveObjectByInstance(name);
        name = nullptr;
    }
    IScene::Terminate();
}

void Save::SaveRecord(int stage) {
    if(Namelen>=0){
        vector<PlayerRecord> playersRecord;

        ifstream file("C:\\Users\\jason\\Downloads\\2025_I2P2_TowerDefense-main\\Resource\\scoreboard.txt");
        string name;
        string times;
        int score;
    
    if(file.is_open()){
        while(file>>name>>score>>times){
            PlayerRecord p;
            p.name = name;
            p.score = score;
            p.time = times;
            playersRecord.push_back(p);
        }
        file.close();
    }

        PlayerRecord p;
        p.name = PlayerName;
        stringstream time;
        std::time_t now = std::time(nullptr);
        std::tm* localTime = std::localtime(&now);
        time << localTime->tm_year+1900<<"-"<<localTime->tm_mon+1<<"-"<<localTime->tm_mday<<"-"<<localTime->tm_hour<<":"<<localTime->tm_min;
        p.time = time.str();
        playersRecord.push_back(p);
        std::sort(playersRecord.begin(), playersRecord.end(), comparePlayers);

        string filename = "Resource/scoreboard.txt";
        ofstream outFile;
        outFile.open(filename, ios::out | ios::trunc);

        if(outFile.is_open()){
            for(auto& record : playersRecord){
                outFile<<record.name<<" "<<record.score<<" "<<record.time<<endl;
            }
            outFile.close();
        }
        else cout<<"Wrong";
        Engine::GameEngine::GetInstance().ChangeScene("stage-select");
    }
    
}


void Save::OnKeyDown(int keyCode){
    IScene::OnKeyDown(keyCode);
    if (keyCode >= ALLEGRO_KEY_A && keyCode <= ALLEGRO_KEY_Z) {
            // 轉換成大寫字母（'A' + (keyCode - ALLEGRO_KEY_A)）
        char pressedChar = 'A' + (keyCode - ALLEGRO_KEY_A);
        PlayerName[Namelen++] = pressedChar;
        PlayerName[Namelen] = '\0';
        std::cout<<pressedChar;
    }
    else if(keyCode==ALLEGRO_KEY_BACKSPACE && Namelen > 0){
        PlayerName[--Namelen] = '\0';
    }
    else if (keyCode >= ALLEGRO_KEY_0 && keyCode <= ALLEGRO_KEY_9) {
        char pressedChar = '0'+keyCode - ALLEGRO_KEY_0;
        std::cout<<pressedChar;
        PlayerName[Namelen++] = pressedChar;
        PlayerName[Namelen] = '\0';
    }
    if(name){
            RemoveObjectByInstance(name);
            name = nullptr;
        }
    if(Namelen>0){
        int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
        int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
        int halfW = w / 2;
        int halfH = h / 2;
        name = new Engine::Label(PlayerName, "pirulen.ttf", 48, halfW, halfH * 3 / 4, 255, 255, 255, 255, 0.5, 0.5);

        AddNewObject(name);
    }
    
}