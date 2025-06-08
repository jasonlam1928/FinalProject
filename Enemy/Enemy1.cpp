#include <string>
#include<iostream>
#include "Enemy1.hpp"
#include"Scene/PlayScene.hpp"

Knight::Knight(int x, int y) : Enemy("play/Knight_01.png", x, y, 10, 100, 5) {
    std::cout<<"Yes"<<std::endl;
}

void Knight::Act(){
    int closetDist=10000;
    int bestStep=10000;
    pair<int, int> bestMove;
    for(auto& r:radius){
        if(radiusStep[r]>distance) continue;
        //cout<<radiusStep[r]<<" "<<distance<<endl;
        for(auto obj:getPlayScene()->UnitGroup->GetObjects()){
            Unit* unit = dynamic_cast<Unit*>(obj);
            if(unit->IsPlayer()){
                int dist = abs(r.first-unit->x0)+abs(r.second-unit->y0);
                if(dist<closetDist){
                    closetDist=dist;
                    bestMove=r;
                    bestStep=radiusStep[r];
                }
                else if(dist==closetDist&&radiusStep[r]<bestStep){
                    bestMove=r;
                    bestStep=radiusStep[r];
                }
            }
            
        }
    }
    x0=bestMove.first, y0=bestMove.second;
    Sprite::Move(bestMove.first*96+96/2, bestMove.second*96+96/2);
    calc=false;
}

