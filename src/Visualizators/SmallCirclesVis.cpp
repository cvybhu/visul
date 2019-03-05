#include "SmallCirclesVis.hpp"

SmallCirclesVis::SmallCirclesVis(const Listener& lis, sf::RenderWindow& win)
    : Visualizator(lis, win)
{
    auto windowSize = window.getSize();
    float circleSize = std::min(windowSize.x, windowSize.y) / 100.f;

    for(int c = 0; c < 60; c++)
    {
        circles.emplace_back();
        Circle& curCircle = circles.back();
        curCircle.sprite = sf::CircleShape(circleSize);
        curCircle.sprite.setOrigin(circleSize, circleSize);
        curCircle.sprite.setPosition(rand()%windowSize.x, rand()%windowSize.y);
        curCircle.sprite.setFillColor(sf::Color(0, 255, 255));

        curCircle.dir = {(float)(rand()%100 - 50) / 100.f, (float)(rand()%100 - 50) / 100.f};
        curCircle.dir *= 5.f;
    }
}

void SmallCirclesVis::update()
{
    for(auto&& circle : circles)
    {
        circle.sprite.move(circle.dir);

        window.draw(circle.sprite);

        auto pos = circle.sprite.getPosition();
        auto windowSize = window.getSize();
        if(pos.x < 0 || pos.x >= windowSize.x)
            circle.dir.x = -circle.dir.x;

        if(pos.y < 0 || pos.y >= windowSize.y)
            circle.dir.y = -circle.dir.y;
    }
}