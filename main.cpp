// [main.cpp]
// This is the entry point of your game.
// You can register your scenes here, and start the game.
#include "Engine/GameEngine.hpp"
#include "Engine/LOG.hpp"
#include "Scene/LoseScene.hpp"
#include "Scene/PlayScene.hpp"
#include "Scene/StageSelectScene.hpp"
#include "Scene/WinScene.hpp"
#include "Scene/StartScene.h"
#include "Scene/SettingsScene.hpp"
#include "Scene/Scoreboard.hpp"
#include "Scene/Save.hpp"

int main(int argc, char **argv) {
	Engine::LOG::SetConfig(true);
	Engine::GameEngine& game = Engine::GameEngine::GetInstance();

    game.AddNewScene("stage-select", new StageSelectScene());
	game.AddNewScene("Settings", new SettingsScene());
	game.AddNewScene("play", new PlayScene());
	game.AddNewScene("lose", new LoseScene());
	game.AddNewScene("win", new WinScene());
	game.AddNewScene("start", new StartScene());
	game.AddNewScene("Scoreboard", new Scoreboard());
	game.AddNewScene("Save", new Save());

    // TODO HACKATHON-1 (1/1): Change the start scene
	game.Start("start", 60, 2200, 1400);
	return 0;
}
