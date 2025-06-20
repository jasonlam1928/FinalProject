#include "IObject.hpp"
#include "Engine/Point.hpp"

namespace Engine {
    IObject::IObject(float x, float y, float w, float h, float anchorX, float anchorY) : Position(Point(x, y)), Size(Point(w, h)), Anchor(Point(anchorX, anchorY)) {}
    std::list<std::pair<bool, IObject *>>::iterator IObject::GetObjectIterator() const {
        return objectIterator;
    }
    void IObject::Draw() const {}
    void IObject::DrawBlue() const{}
    void IObject::Drawhp(int hp, int maxhp, int Level) const{}
    void IObject::Update(float deltaTime) {}
}
