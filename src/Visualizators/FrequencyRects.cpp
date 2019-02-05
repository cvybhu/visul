#include "FrequencyRects.hpp"
#include <string>

FrequencyRects::FrequencyRects(const Listener& lis, sf::RenderWindow& win)
    : Visualizator(lis, win)
{
    frequencies = {20, 50, 100, 500, 1000, 5000, 10000, 20000};

    auto windowSize = window.getSize();
    

    float textHeight = windowSize.y/8.f;

    float n = frequencies.size();
    float rectWidth = windowSize.x / n * 0.7;
    float rectHeight = (windowSize.y - textHeight)*0.9;
    float margin = (windowSize.x / n - rectWidth)/2.f;

    font.loadFromFile("data/arial.ttf");

    for(int i = 0; i < (int)frequencies.size(); i++)
    {
        sf::RectangleShape rect(sf::Vector2f(rectWidth, rectHeight));;
        rect.setOrigin(rectWidth/2, rectHeight);
        rect.setPosition(sf::Vector2f(margin+i * (windowSize.x/n) + rectWidth/2, windowSize.y - textHeight));
        rect.setFillColor(sf::Color(0, 255, 255));

        rects.emplace_back(rect);


        texts.emplace_back(std::to_string(frequencies[i]) + "Hz", font);
        sf::Text& text = texts.back();
        text.setCharacterSize((float)textHeight/4.0);
        text.setPosition(sf::Vector2f(margin+i * (windowSize.x/n), windowSize.y - textHeight));
        text.setFillColor(sf::Color(255, 255, 255));
    }
}


void FrequencyRects::update()
{
    for(int i = 0; i < (int)frequencies.size(); i++)
    {   
        int fromHz = frequencies[i];
        int toHz = (i+1 == (int)frequencies.size() ? 20*1000 : frequencies[i+1]-1);
        float intensity = listener.getAvgFrequency(fromHz, toHz);

        rects[i].setScale(sf::Vector2f(1, intensity));
    }

    for(auto& r : rects)
        window.draw(r);

    for(auto& t : texts)
        window.draw(t);
}