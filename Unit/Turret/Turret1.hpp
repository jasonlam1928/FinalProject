#include"Turret.hpp"

class Knight1 : public Turret {
public:
    Knight1(int x, int y);
    void Act() override;
};