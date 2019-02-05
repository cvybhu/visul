#pragma once

#include <vector>
#include <deque>
#include <thread>
#include <atomic>
#include <shared_mutex>
#include <pulse/simple.h>

class Listener
{
public:
    Listener();

    bool isSilent() const;
    bool wasSpike() const;
    float getAverageVolume() const;
    float getAvgFrequency(int fromHz, int toHz) const; // averages frequencies intensity
    float getMaxFrequency(int fromHz, int toHz) const; // gets maximum intensity of frequencies in range

    ~Listener();
private:
//Basic functionality
    static const int sampleRate = 44100;
    pa_simple *simpleAPI;

    std::deque<float> buff;
    int maxBuffSize;

    void mainThreadFunc();
    std::unique_ptr<std::thread> listenThread;
    std::atomic<bool> isTimeToStop;

//Sound info
    std::atomic<bool> somethingPlays;
    std::atomic<float> averageVolume;
    std::atomic<bool> spikeHappened;


    static const int minFrequecy = 20; 
    static const int maxFrequency = 20*1000;
    std::vector<float> frequencies; //frequency intensities
    std::vector<float> frequenciesPrefixSum;
    mutable std::shared_mutex frequenciesLock;

    std::vector<std::pair<int,int> > frequencyGroups;
    std::vector<float> frequencyGroupsAvgerageAverage;
};