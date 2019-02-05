#include "AvgVolumeCircle.hpp"

AvgVolumeCircle::AvgVolumeCircle(const Listener& lis, sf::RenderWindow& win)
    : Visualizator(lis, win)
{
    auto windowSize = window.getSize();
    float circleSize = std::min(windowSize.x, windowSize.y) / 2.f;

    circle = sf::CircleShape(circleSize);
    circle.setOrigin(circleSize, circleSize);
    circle.setPosition(windowSize.x/2.f, windowSize.y/2.f);
    circle.setFillColor(sf::Color(0, 255, 255));
}

void AvgVolumeCircle::update()
{
    float avgVolume = listener.getAverageVolume() / 10.f;
    circle.setScale(sf::Vector2f(avgVolume, avgVolume));
    window.draw(circle);
}