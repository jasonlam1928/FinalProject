#include "Unit.hpp"
#include "Scene/PlayScene.hpp"
#include "Engine/GameEngine.hpp"
#include <allegro5/allegro_primitives.h>

using Engine::IntPoint;

PlayScene* Unit::getPlayScene() {
    return dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetActiveScene());
}

Unit::Unit(float x, float y, std::string img, float speed, float hp, int distance)
    : Sprite(img, x, y), Speed(speed), HP(hp), ActionValue(MaxActionValue), distance(distance), calc(false) {
    gridPos = IntPoint(x / 96, y / 96);
}

bool Unit::UpdateActionValue(float deltaTime) {
    ActionValue -= Speed;
    if (ActionValue <= 0) {
        ActionValue += MaxActionValue;
        return true;
    }
    return false;
}

void Unit::drawRadius(int cameraX, int cameraY) {
    if (!calc) {
        int dx[4] = {0, 0, 1, -1};
        int dy[4] = {1, -1, 0, 0};

        std::queue<IntPoint> q;
        std::set<IntPoint> visited;
        std::map<IntPoint, int> level;
        std::map<IntPoint, bool> valid;

        q.push(gridPos);
        visited.insert(gridPos);
        level[gridPos] = 0;
        valid[gridPos] = true;

        int step = 0;
        while (!q.empty() && step <= distance) {
            int sz = q.size();
            for (int i = 0; i < sz; ++i) {
                IntPoint cur = q.front(); q.pop();
                for (int d = 0; d < 4; ++d) {
                    IntPoint nxt = cur + IntPoint(dx[d], dy[d]);
                    bool outOfRange = false;
                    if (visited.count(nxt)) continue;

                    if (nxt.x <= 1 || nxt.x >= 62 || nxt.y <= 1 || nxt.y >= 62) {
                        outOfRange = true;
                        valid[nxt] = false;
                    }
                    if (step == distance) {
                        outOfRange = true;
                        valid[nxt] = false;
                    }

                    for (auto& obj : getPlayScene()->UnitGroup->GetObjects()) {
                        auto unit = dynamic_cast<Unit*>(obj);
                        if (nxt == unit->gridPos) {
                            outOfRange = true;
                            valid[nxt] = false;
                            break;
                        }
                    }

                    visited.insert(nxt);
                    level[nxt] = step + 1;
                    if (!outOfRange) {
                        q.push(nxt);
                        valid[nxt] = true;
                    }
                }
            }
            ++step;
        }

        radius = visited;
        radiusStep = level;
        Valid = valid;
        drawStep = 0;
        drawTimer = 0;
        calc = true;
    }

    for (auto& r : radius) {
        int dist = radiusStep[r];
        if (dist > drawStep) continue;

        float x = r.x * 96 - cameraX;
        float y = r.y * 96 - cameraY;
        ALLEGRO_COLOR fillColor = Valid[r] ?
            al_map_rgba(144, 238, 144, 120) :
            al_map_rgba(238, 144, 144, 200);
        al_draw_filled_rectangle(x, y, x + 96, y + 96, fillColor);
        al_draw_rectangle(x, y, x + 96, y + 96, al_map_rgb(0, 128, 0), 1);
    }
}

void Unit::UpdateRadiusAnimation(float deltaTime) {
    drawTimer += deltaTime;
    if (drawTimer >= drawInterval) {
        drawStep++;
        drawTimer = 0;
    }
}

bool Unit::CheckPlacement(int x, int y) {
    IntPoint p(x, y);
    if (!Valid[p]) return false;

    for (auto& r : radius) {
        if (r == p) {
            previewPos = p;
            Sprite::Move(p.x * 96 + 48, p.y * 96 + 48);
            return true;
        }
    }
    return false;
}

void Unit::MovetoPreview() {
    gridPos = previewPos;
}

void Unit::CancelPreview() {
    Sprite::Move(gridPos.x * 96 + 48, gridPos.y * 96 + 48);
}
