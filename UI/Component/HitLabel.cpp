#include <allegro5/allegro_font.h>
#include <memory>
#include <cstdlib> 
#include <ctime>  

#include "Engine/IObject.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "Engine/GameEngine.hpp"
#include "HitLabel.hpp"
#include "Scene/PlayScene.hpp"

PlayScene* Engine::HitLabel::getPlayScene() {
    return dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetActiveScene());
}

namespace Engine {
    HitLabel::HitLabel(const std::string &text, const std::string &font, float x, float y, float anchorX, float anchorY) : IObject(x + rand()%50-25, y+rand()%50-25, 0, 0, anchorX, anchorY), font(Resources::GetInstance().GetFont(font,24)), Text(text) {
        Timer = 20;
        Color = al_map_rgba(255, 0 ,0, 255);
    }
    void HitLabel::Draw() const {
        al_draw_text(font.get(), Color, Position.x - Anchor.x * GetTextWidth(), Position.y - Anchor.y * GetTextHeight(), 0, Text.c_str());
    }

    void HitLabel::Update(float deltaTime){
        Timer-=deltaTime;
        if(Timer<=0){
            getPlayScene()->UIGroup->RemoveObject(objectIterator);
        }
    }

    int HitLabel::GetTextWidth() const {
        return al_get_text_width(font.get(), Text.c_str());
    }
    int HitLabel::GetTextHeight() const {
        return al_get_font_line_height(font.get());
    }
}
