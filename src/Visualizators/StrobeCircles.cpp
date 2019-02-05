#include "StrobeCircles.hpp"
#include <cmath>
#include <iostream>

StrobeCircles::StrobeCircles(const Listener& lis, sf::RenderWindow& win)
    : Visualizator(lis, win)
{
    lastWasWhite = false;
    startTime = clock.now();
}

void StrobeCircles::update()
{
    bool white = listener.wasSpike() && !lastWasWhite; 
    lastWasWhite = white;

    auto windowSize = window.getSize();

    if(white)
    {
        int circleDiff = 100; //Could be smaller if sfml didnt suck so much
        float moveSpeed = 3;

        float timeInS = std::chrono::duration<float>(clock.now() - startTime).count();
        for(int i = sqrt(windowSize.x * windowSize.y) / circleDiff + 10; i >= 1; i--)
        {
            float circleSize = i * circleDiff;
            sf::CircleShape circle(circleSize);

            circle.setFillColor(i%2 ? sf::Color::Black : sf::Color::White);
            circle.setOrigin(circleSize, circleSize);
            circle.setPosition(sf::Vector2f(windowSize.x/2, windowSize.y/2));

            float scaleAtEnd = std::max(0.f, 1.f - 2 * (float)circleDiff / circleSize);
            float scaleTime = fmod(moveSpeed * timeInS, 1); // 0 ---> 1 
            float scale = 1.f * (1.f - scaleTime) + scaleAtEnd * scaleTime;
            circle.setScale(sf::Vector2f(scale, scale));

            window.draw(circle);
        }

    }
    else
        window.clear(sf::Color::Black);
}