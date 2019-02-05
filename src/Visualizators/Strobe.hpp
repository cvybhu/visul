#pragma once

#include "Visualizator.hpp"

class Strobe : public Visualizator
{
public:
    Strobe(const Listener&, sf::RenderWindow&);

    void update();

private:
    bool lastWasWhite;
};