#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <future>
#include <deque>
#include <condition_variable>
#include "TrafficObject.h"

// forward declarations to avoid include cycle
class Vehicle;

template <class T>
class MessageQueue
{
public:
    
    void send(T &&msg);
    T receive();


private:
    std::deque<T> _queue;
    std::condition_variable cond;
    std::mutex _mutex;
};

enum TrafficLightPhase {
    red, green
};

class TrafficLight : public TrafficObject
{
public:
    // constructor / desctructor
    TrafficLight();
    //~TrafficLight();

    // getters / setters
    TrafficLightPhase getCurrentPhase();
    //void setCurrentPhase(TrafficLightPhase currentPhase);
    
    // typical behaviour methods
    void waitForGreen();
    void simulate();
    

private:
    // typical behaviour methods

    MessageQueue<TrafficLightPhase> messages;

    std::condition_variable _condition;
    std::mutex _mutex;
    TrafficLightPhase _currentPhase;

    void cycleThroughPhases();
};

#endif