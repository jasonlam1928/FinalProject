#include"Player.hpp"

class Knight1 : public Player {
public:
    Knight1(int x, int y);
    bool Act() override;
    void chooseSkill() override;
    Unit* Clone() override { return new Knight1(*this); }
};