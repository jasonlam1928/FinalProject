#include "Engine/IScene.hpp"
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <functional>
#include <memory>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;
class Scoreboard final : public Engine::IScene {
public:
    explicit Scoreboard() = default;
    void UpdateScorePage();
    void Initialize() override;
    void Terminate() override;
    void BackOnClick(int stage);
    void NextPage(int type);
    void PrevPage(int type);
};
struct PlayerRecord{
    string name;
    int score;
    string time;
};
vector<PlayerRecord> getPlayerRecord();