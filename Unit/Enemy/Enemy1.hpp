#include"Enemy.hpp"

class Knight : public Enemy {
public:
    Knight(int x, int y);
    bool Act() override;
    void chooseSkill() override;
};