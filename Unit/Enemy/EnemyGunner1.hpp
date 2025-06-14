#include"Enemy.hpp"

class EnemyGunner1 : public Enemy {
public:
    EnemyGunner1(int x, int y);
    bool Act() override;
    void chooseSkill() override;
    Unit* Clone() override { return new EnemyGunner1(*this); }
};