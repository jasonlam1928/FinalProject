#ifndef MAPEDITORSELECTSCENE_HPP
#define MAPEDITORSELECTSCENE_HPP
#include <allegro5/allegro_audio.h>
#include <memory>

#include "Engine/IScene.hpp"

class MapEditorSelectScene final : public Engine::IScene {
private:

public:
    explicit MapEditorSelectScene() = default;
    void Initialize() override;
    void Terminate() override;
    void PlayOnClick(int stage);
    void Draw() const override;
    void ScoreboardOnClick(int stage);
    void BackOnClick(int stage);

    void BGMSlideOnValueChanged(float value);
    void SFXSlideOnValueChanged(float value);
};

#endif   // MAPEDITORSELECTSCENE_HPP
