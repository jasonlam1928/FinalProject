#include "Engine/IScene.hpp"
#include <allegro5/allegro_audio.h>
#include <memory>

class Save final : public Engine::IScene {
private:
    std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> bgmInstance;

public:
    explicit Save() = default;
    void Initialize() override;
    void Terminate() override;
    void SaveRecord(int stage);
    void OnKeyDown(int keyCode) override;
};