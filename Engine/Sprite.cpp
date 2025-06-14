#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <string>         // std::to_string
#include <allegro5/allegro_font.h>   // ALLEGRO_FONT
#include <allegro5/allegro_ttf.h>  

#include <memory>

#include "Sprite.hpp"
#include "UI/Component/Image.hpp"
#include "Engine/Group.hpp"
#include "Engine/IObject.hpp"
#include "Engine/IScene.hpp"
#include "Engine/Point.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Resources.hpp"

std::shared_ptr<ALLEGRO_FONT> font1;

namespace Engine {
    Sprite::Sprite(std::string img, float x, float y, float w, float h, float anchorX, float anchorY,
                   float rotation, float vx, float vy, unsigned char r, unsigned char g, unsigned char b, unsigned char a) : Image(img, x, y, w, h, anchorX, anchorY), Rotation(rotation), Velocity(Point(vx, vy)), Tint(al_map_rgba(r, g, b, a)) {
    }
    void Sprite::Draw() const {
        al_draw_tinted_scaled_rotated_bitmap(bmp.get(), Tint, Anchor.x * GetBitmapWidth(), Anchor.y * GetBitmapHeight(),
                                             Position.x, Position.y, Size.x / GetBitmapWidth(), Size.y / GetBitmapHeight(), Rotation, 0);
    }
    void Sprite::Draw(float scale) const {
        al_draw_tinted_scaled_rotated_bitmap(
            bmp.get(), Tint,
            Anchor.x * GetBitmapWidth(), Anchor.y * GetBitmapHeight(),
            Position.x, Position.y,
            (Size.x / GetBitmapWidth()) * scale,
            (Size.y / GetBitmapHeight()) * scale,
            Rotation, 0);
    }
    void Sprite::DrawBlue() const {
        ALLEGRO_COLOR Tint1 = al_map_rgba_f(0.3, 0.5, 1.0, 1.0);
        al_draw_tinted_scaled_rotated_bitmap(bmp.get(), Tint1, Anchor.x * GetBitmapWidth(), Anchor.y * GetBitmapHeight(),
                                             Position.x, Position.y, Size.x / GetBitmapWidth(), Size.y / GetBitmapHeight(), Rotation, 0);
    }

    void Sprite::Drawhp(int hp, int maxhp, int Level)const{
        if(hp==0) return;
        const float barWidth = 40;
        const float barHeight = 5;
        const float barX = Position.x - barWidth / 2;
        const float barY = Position.y + Size.y / 2 - 5;

        al_draw_filled_rectangle(barX, barY, barX + barWidth, barY + barHeight, al_map_rgb(100, 100, 100));
        float hpRatio = static_cast<float>(hp) / maxhp;
        ALLEGRO_COLOR hpColor = al_map_rgb(255 * (1 - hpRatio), 255 * hpRatio, 0);
        al_draw_filled_rectangle(barX, barY, barX + barWidth * hpRatio, barY + barHeight, hpColor);

        al_draw_rectangle(barX, barY, barX + barWidth, barY + barHeight, al_map_rgb(0, 0, 0), 1);

        font1 = Resources::GetInstance().GetFont("pirulen.ttf",12);
        std::string hpText = std::to_string(hp) + "/" + std::to_string(maxhp);
        std::string LevelText = "Level:"+std::to_string(Level);
        float textX = barX + barWidth / 2;
        float textY = barY - 5; // 血條上方
        
        al_draw_text(font1.get(), al_map_rgb(255, 255, 255), textX, textY, ALLEGRO_ALIGN_CENTER, hpText.c_str());
        al_draw_text(font1.get(), al_map_rgb(255, 255, 255), textX, textY-10, ALLEGRO_ALIGN_CENTER, LevelText.c_str());

    }

    void Sprite::Update(float deltaTime) {
        Position.x += Velocity.x * deltaTime;
        Position.y += Velocity.y * deltaTime;
    }

    void Sprite::Move(int x, int y){
        Position.x=x;
        Position.y=y;
    }
};
