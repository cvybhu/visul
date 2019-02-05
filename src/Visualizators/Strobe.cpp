#include "Strobe.hpp"

Strobe::Strobe(const Listener& lis, sf::RenderWindow& win)
    : Visualizator(lis, win)
{
    lastWasWhite = false;
}

void Strobe::update()
{
    bool white = listener.wasSpike() && !lastWasWhite; 

    if(white)
        window.clear(sf::Color::White);
    else
        window.clear(sf::Color::Black);

    lastWasWhite = white;
    
}