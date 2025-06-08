#include<set>
#include<queue>
#include<iostream>
#include<map>
using namespace std;
#include <allegro5/allegro_primitives.h>
#include"Scene/PlayScene.hpp"
#include"Engine/GameEngine.hpp"
#include"Unit.hpp"

PlayScene *Unit::getPlayScene() {
    return dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetActiveScene());
}

bool Unit::UpdateActionValue(float deltaTime){
    ActionValue -= Speed ;
    if (ActionValue <= 0) {
        ActionValue += MaxActionValue; // 重設行動
        return true;
    }
    return false;
}

Unit::Unit(float x, float y, std::string img, float speed, float hp, int distance): Sprite(img, x, y), Speed(speed), HP(hp), ActionValue(MaxActionValue), distance(distance){
    x0=x/96;
    y0=y/96;
    calc = false;
}

void Unit::drawRadius(int cameraX, int cameraY){
    if(!calc){
        int dx[4]={0,0,1,-1};
        int dy[4]={1,-1,0,0};
        map<pair<int,int>, int> level;
        queue<pair<int,int>> q;
        set<pair<int,int>> visited;
        q.push({x0,y0});
        visited.insert({x0,y0});
        level[{x0, y0}] = 0;
        int step=0;
        while(!q.empty()&&step<=distance){
            int sz=q.size();
            //cout<<"sz:"<<sz<<endl;
            for(int i=0;i<sz;i++){
                pair<int,int> cur = q.front();
                q.pop();
                for(int i=0;i<4;i++){
                    auto nxt = cur;
                    nxt.first+=dx[i];
                    nxt.second+=dy[i];
                    bool valid=true;
                    if(nxt.first<=1||nxt.first>=62||nxt.second<=1||nxt.second>=62){
                        valid=false;
                        visited.insert(nxt);
                        level[nxt]=distance+1;
                    }
                    if(visited.count(nxt)) continue;
                    for(auto& obj:getPlayScene()->UnitGroup->GetObjects()){
                        auto unit=dynamic_cast<Unit*>(obj);
                        if(nxt.first==unit->x0&&nxt.second==unit->y0){
                            valid = false;
                            visited.insert(nxt);
                            level[nxt]=distance+1;
                            break;
                        }
                    }
                    if(!valid) continue;
                    //cout<<"x:"<<nxt.first<<" y:"<<nxt.second<<endl;
                    visited.insert(nxt);
                    q.push(nxt);
                    level[nxt] = step+1;
                        
                }
            }
            step++;
        }
            radius=visited;
            radiusStep=level;
            drawStep = 0;
            drawTimer = 0;
            calc=true;
    }
    
    for (auto& r : radius) {
        int dist = radiusStep[r];
        if (dist > drawStep) continue; // 未到動畫時機，跳過

        float x = r.first * 96 - cameraX;
        float y = r.second * 96 - cameraY;
        ALLEGRO_COLOR fillColor;
        if(dist>=distance+1) fillColor = al_map_rgba(238, 144, 144, 200);
        else fillColor = al_map_rgba(144, 238, 144, 120);
        al_draw_filled_rectangle(x, y, x + 96, y + 96, fillColor);

        ALLEGRO_COLOR borderColor = al_map_rgb(0, 128, 0);
        al_draw_rectangle(x, y, x + 96, y + 96, borderColor, 1);
    }
    
}
void Unit::UpdateRadiusAnimation(float deltaTime) {
    drawTimer += deltaTime;
    if (drawTimer >= drawInterval) {
        drawStep++;
        drawTimer = 0;
    }
}

bool Unit::CheckPlacement(int x, int y){
    for(auto& r:radius){
        if(radiusStep[r]>distance)continue;
        if(x==r.first&&y==r.second){
            Prex0=x;
            Prey0=y;
            Sprite::Move(x*96+96/2, y*96+96/2);
            return true;
        }
    }
    return false;
}

void Unit::MovetoPreview(){
    x0=Prex0, y0=Prey0;
}

void Unit::CancelPreview(){
    Sprite::Move(x0*96+96/2, y0*96+96/2);
}