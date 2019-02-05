#pragma once

#include "Visualizator.hpp"

class FrequencyRects : public Visualizator
{
public:
    FrequencyRects(const Listener&, sf::RenderWindow&);

    void update();

private:
    std::vector<int> frequencies;
    
    std::vector<sf::RectangleShape> rects;

    sf::Font font;
    std::vector<sf::Text> texts;
};