#include <iostream>
#include <SFML/Graphics.hpp>
#include "Listener/Listener.hpp"

//Visualizators:
#include "Visualizators/AvgVolumeCircle.hpp"
#include "Visualizators/FrequencyRects.hpp"
#include "Visualizators/Strobe.hpp"
#include "Visualizators/StrobeCircles.hpp"
#include "Visualizators/SmallCirclesVis.hpp"

using namespace std;


int main() 
{
//Setup Listener
    Listener lis;
    
//Setup main visul window
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "VISUL", sf::Style::None);
    int refreshRate = 30;
    window.setFramerateLimit(refreshRate);

//Setup vizualizators
    AvgVolumeCircle avgVolumeCircleVis(lis, window);
    FrequencyRects frequencyRectsVis(lis, window);
    Strobe strobeVis(lis, window);
    StrobeCircles strobeCirclesVis(lis, window);
    SmallCirclesVis smallCirclesVis(lis, window);


    Visualizator* curVisualizator = &strobeCirclesVis;

//Go
    std::chrono::high_resolution_clock frameClock;
    auto lastFrameTimeWrite = frameClock.now();
    double maxFrameTime = 0;

    while (window.isOpen())
    {
        auto frameStartTime = frameClock.now();

        //Events
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
        {
            window.close();
            break;
        }


        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }   
        window.clear();

        //Drawing
        if(curVisualizator)
            curVisualizator->update();

        //measure frame time
        double frameTime = std::chrono::duration<double>(frameClock.now() - frameStartTime).count();
        maxFrameTime = std::max(maxFrameTime, frameTime);
        
        if(std::chrono::duration<double>(frameClock.now() - lastFrameTimeWrite).count() > 0.5)
        {
            std::cout << "Max frame time: " << 1000.f*maxFrameTime << "ms / " << 1000.f*(1.0/(double)refreshRate)<< "ms\n";
            lastFrameTimeWrite = frameClock.now();
            maxFrameTime = 0.f;
        }

        window.display();
    }

    return 0;
}