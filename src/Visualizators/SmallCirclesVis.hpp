#pragma once
#include "Visualizator.hpp"
#include <vector>

class SmallCirclesVis : public Visualizator
{
public:
    SmallCirclesVis(const Listener&, sf::RenderWindow&);

    void update();

private:
    struct Circle
    {
        sf::CircleShape sprite;
        sf::Vector2f dir;
    };

    std::vector<Circle> circles;
};


