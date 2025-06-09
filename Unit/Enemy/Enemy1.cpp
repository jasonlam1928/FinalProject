#include <string>
#include<iostream>
#include "Enemy1.hpp"
#include"Scene/PlayScene.hpp"
#include"Engine/IntPoint.hpp"

Knight::Knight(int x, int y) : Enemy("play/Knight_01.png", x, y, 10, 100, 5) {
    std::cout<<"Yes"<<std::endl;
}

void Knight::Act(){
    int closetDist=10000;
    int bestStep=10000;
    Engine::IntPoint bestMove;
    for(auto& r:radius){
        if(!Valid[r]) continue;
        //cout<<radiusStep[r]<<" "<<distance<<endl;
        for(auto obj:getPlayScene()->UnitGroup->GetObjects()){
            Unit* unit = dynamic_cast<Unit*>(obj);
            if(unit->IsPlayer()){
                int dist = abs(r.x - unit->gridPos.x)+abs(r.y - unit->gridPos.y);
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
    gridPos.x=bestMove.x, gridPos.y=bestMove.y;
    Sprite::Move(bestMove.x*96+96/2, bestMove.y*96+96/2);
    calc=false;
}

