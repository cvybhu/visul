#include "Listener.hpp"
#include <iostream>
#include <cmath>
#include <cassert>
#include <pulse/error.h>
#include <fftw3.h>


Listener::Listener()
     : isTimeToStop(false)
{

//Initialize pulseaudio API
    int endianCheck = 1;
    bool isBigEndian = ((*((char*)&endianCheck)) == 0);

    static const pa_sample_spec sampleSpecs = {
        .format = (isBigEndian ? PA_SAMPLE_FLOAT32BE : PA_SAMPLE_FLOAT32LE),
        .rate = sampleRate,
        .channels = 1
    };

    int error;
    simpleAPI = pa_simple_new(NULL, 
                              "./main", 
                              PA_STREAM_RECORD, 
                              NULL, 
                              "record", 
                              &sampleSpecs, 
                              NULL, 
                              NULL, 
                              &error);
    if(!simpleAPI)
    {
        std::cerr << __FILE__": pa_simple_new() failed: " <<  pa_strerror(error) << "\n";
        std::exit(1);
    }

//Setup stuff
    maxBuffSize = 1300; // To hold 0.5s of audio

    frequencies.resize(maxFrequency+1);
    frequencyGroups = {{20, 300}, {300, 500}, {500, 800}, {800, 1200}, {1200, 5000}, {1500, 20*1000}};
    frequencyGroupsAvgerageAverage.resize(frequencyGroups.size(), 0.f);


//Start listening thread
    listenThread = std::make_unique<std::thread>([this](){this->mainThreadFunc();});
}




void Listener::mainThreadFunc()
{
    int error;

    std::vector<double> dftIn(maxBuffSize);
    fftw_complex* dftOut = (fftw_complex*)fftw_malloc(maxBuffSize * sizeof(fftw_complex));
    fftw_plan dftPlan = fftw_plan_dft_r2c_1d(maxBuffSize, dftIn.data(), dftOut, 0);


    std::vector<float> readBuff(maxBuffSize); //Reads size() samples of audio -> processes

    while(!isTimeToStop)
    {
    //Read next buffer of data
        if(pa_simple_read(simpleAPI, readBuff.data(), readBuff.size() * sizeof(float), &error) < 0)
        {
            std::cerr << __FILE__": pa_simple_read() failed: %s\n" << pa_strerror(error) << '\n';
            std::exit(1);
        }

        for(int i = 0; i < (int)readBuff.size(); i++)
            buff.emplace_back(readBuff[i]);

        while((int)buff.size() > maxBuffSize)
            buff.pop_front();

    //Process it

        //Check if something plays
        {
            bool all0s = true;
            for(int i = 0; i < (int)buff.size(); i++)
                if(buff[i] != 0)
                {
                    all0s = false;
                    break;
                }

            somethingPlays = !all0s;
        }

        //Get average volume
        float avgVolume = 0.f; //average amplitude
        {
            for(float& a : buff)
                avgVolume += fabs(a);

            avgVolume /= (float)buff.size();

            averageVolume = 20 * log(1.f+avgVolume); //to dB
        }


        //Get frequencies with Discrete Fourier Transform
        {
            for(int i = 0; i < maxBuffSize; i++)
                dftIn[i] = buff[i%buff.size()];
            
            fftw_execute(dftPlan);

            for(auto& f : frequencies)
                f = 0.f;

            frequenciesLock.lock();

            int lastFrequency = 0;
            for(int i = 1; i < (int)buff.size()/2; i++)
            {
                float curFrequency = (float)i * ((float)sampleRate / (float)buff.size());
                float intensity = sqrt(dftOut[i][0]*dftOut[i][0] + dftOut[i][1]*dftOut[i][1]);
                intensity /= (float)maxBuffSize;

                if(avgVolume != 0)
                    intensity = log(1.f+intensity) * sqrt(curFrequency) / avgVolume / 10.f;

                if(minFrequecy <= (int)curFrequency && (int)curFrequency <= maxFrequency)
                {
                    frequencies[(int)curFrequency] = intensity;
                    //Lineary interpolate these in between
                    for(int f = lastFrequency+1; f < (int)curFrequency; f++)
                    {
                        float dist = (int)curFrequency - lastFrequency;
                        float ratio = (float)(f-lastFrequency)/dist;
                        frequencies[f] = ratio * frequencies[lastFrequency] 
                                       + (1.f - ratio) * frequencies[(int)curFrequency];
                    }
                }

                lastFrequency = curFrequency;
            }

            frequenciesLock.unlock();
        }


        //averageAverage for spike detecting
        {
            spikeHappened = false;

            auto newAverageAverage = [](float avgAverage, float newAvg) -> float
            {
                float updateRatio = 0.3;
                return newAvg * updateRatio + avgAverage * (1.f - updateRatio);
            };

            for(int i = 0; i < (int)frequencyGroups.size(); i++)
            {
                auto& curGroup = frequencyGroups[i];
                float& curAverageAverage = frequencyGroupsAvgerageAverage[i];
                float curAverage = getAvgFrequency(curGroup.first, curGroup.second);

                curAverageAverage = newAverageAverage(curAverageAverage, curAverage);


                if(curAverage > 1.2 * curAverageAverage)
                    spikeHappened = true;
            }
        }

    }




    fftw_destroy_plan(dftPlan);
    fftw_free(dftOut);
}





Listener::~Listener()
{
    if(simpleAPI)
        pa_simple_free(simpleAPI);

    if(listenThread != nullptr)
    {
        isTimeToStop = true;
        listenThread->join();
    }
}


// PUBLIC FUNCS \/ \/ \/ -

bool Listener::isSilent() const
    {return !somethingPlays;}

float Listener::getAverageVolume() const
    {return averageVolume;}

float Listener::getAvgFrequency(int fromHz, int toHz) const
{
    assert(minFrequecy <= fromHz && toHz <= maxFrequency);

    frequenciesLock.lock_shared();

    float sum = 0.f;
    for(int f = fromHz; f <= toHz; f++)
        sum += frequencies[f];

    frequenciesLock.unlock_shared();

    return sum / (float)(toHz - fromHz + 1);
}

float Listener::getMaxFrequency(int fromHz, int toHz) const
{
    assert(minFrequecy <= fromHz && toHz <= maxFrequency);

    frequenciesLock.lock_shared();

    float res = 0.f;
    for(int f = fromHz; f <= toHz; f++)
        res = std::max(res, frequencies[f]);

    frequenciesLock.unlock_shared();

    return res;
}

bool Listener::wasSpike() const
{
    return spikeHappened;
}