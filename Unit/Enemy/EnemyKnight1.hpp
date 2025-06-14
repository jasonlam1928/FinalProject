#include"Enemy.hpp"

class EnemyKnight1 : public Enemy {
public:
    EnemyKnight1(int x, int y);
    bool Act() override;
    void chooseSkill() override;
    Unit* Clone() override { return new EnemyKnight1(*this); }
};