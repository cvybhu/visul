#pragma once
#include "Visualizator.hpp"

class AvgVolumeCircle : public Visualizator
{
public:
    AvgVolumeCircle(const Listener&, sf::RenderWindow&);

    void update();

private:
    sf::CircleShape circle;
};


