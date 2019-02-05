#pragma once

#include "Visualizator.hpp"
#include <chrono>

class StrobeCircles : public Visualizator
{
public:
    StrobeCircles(const Listener&, sf::RenderWindow&);

    void update();

private:
    bool lastWasWhite;

    std::chrono::high_resolution_clock clock;
    decltype(clock.now()) startTime;
};