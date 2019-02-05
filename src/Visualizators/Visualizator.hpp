#pragma once
#include "Listener/Listener.hpp"
#include <SFML/Graphics.hpp>

/*
    Base Visualizator class
    update() draws stuf - should be called every frame
    visualizer assumes screen is cleared black
*/

class Visualizator
{
public:
    Visualizator(const Listener&, sf::RenderWindow&);

    virtual void update() = 0;

    virtual ~Visualizator() = default;

protected:
    const Listener& listener;
    sf::RenderWindow& window;
};