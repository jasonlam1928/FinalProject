#include <functional>
#include <memory>

#include "Engine/Collider.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "Image.hpp"
#include "ImageButton.hpp"

namespace Engine {
    ImageButton::ImageButton(std::string img, std::string imgIn, float x, float y, float w, float h, float anchorX, float anchorY) : Image(img, x, y, w, h, anchorX, anchorY), imgOut(Resources::GetInstance().GetBitmap(img)), imgIn(Resources::GetInstance().GetBitmap(imgIn)) {
        Point mouse = GameEngine::GetInstance().GetMousePosition();
        mouseIn = Collider::IsPointInBitmap(Point((mouse.x - Position.x) * GetBitmapWidth() / Size.x + Anchor.x * GetBitmapWidth(), (mouse.y - Position.y) * GetBitmapHeight() / Size.y + Anchor.y * GetBitmapHeight()), bmp);
        if (!mouseIn || !Enabled) bmp = imgOut;
        else bmp = this->imgIn;
        cameraX=cameraY=0;
    }
    void ImageButton::SetOnClickCallback(std::function<void(void)> onClickCallback) {
        OnClickCallback = onClickCallback;
    }
    void ImageButton::OnMouseDown(int button, int mx, int my) {
        if ((button & 1) && mouseIn && Enabled) {
            if (OnClickCallback)
                OnClickCallback();
        }
    }
    void ImageButton::OnMouseMove(int mx, int my) {
        mouseIn = Collider::IsPointInBitmap(Point((mx - Position.x+cameraX) * GetBitmapWidth() / Size.x + Anchor.x * GetBitmapWidth(), (my - Position.y+cameraY) * GetBitmapHeight() / Size.y + Anchor.y * GetBitmapHeight()), bmp);
        if (!mouseIn || !Enabled) bmp = imgOut;
        else bmp = imgIn;
    }
    void ImageButton::SetPosition(float x, float y, int cx, int cy) {
        // 直接修改继承自 Image 的 Position 字段
        Position.x = x;
        Position.y = y;
        cameraX=cx;
        cameraY=cy;

    }
}
