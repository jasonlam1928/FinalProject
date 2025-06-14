#include"Player.hpp"

class Gunner1 : public Player {
public:
    Gunner1(int x, int y);
    bool Act() override;
    void chooseSkill() override;
    Unit* Clone() override { return new Gunner1(*this); }
};