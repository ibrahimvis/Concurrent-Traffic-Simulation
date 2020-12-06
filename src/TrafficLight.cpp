#include <iostream>
#include <random>
#include "TrafficLight.h"


using std::chrono::system_clock;
using std::chrono::duration_cast;
using std::chrono::milliseconds;


/* Implementation of class "MessageQueue" */


template <typename T>
T MessageQueue<T>::receive()
{

    std::unique_lock<std::mutex> lock(_mutex);
    cond.wait(lock, [this] { return !_queue.empty(); });

    T m = std::move(_queue.front());
    _queue.pop_front();

    return m;
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{

    std::lock_guard<std::mutex> lock(_mutex);
    
    // for performance for traffic signs
    _queue.clear();

    _queue.emplace_back(std::move(msg));
    cond.notify_one();

}


/* Implementation of class "TrafficLight" */


TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

void TrafficLight::waitForGreen()
{

    while (true) {
        if(messages.receive() == TrafficLightPhase::green)
            return;
    }
}


TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}


void TrafficLight::simulate()
{
    
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{

    system_clock::time_point before = system_clock::now();
    
    /* Init our random generation between 4 and 6 seconds */
    std::random_device rd;
    std::mt19937 eng(rd());
    std::uniform_int_distribution<> distr(4000, 6000);
    /* Initalize variables */
    int cycle_duration = distr(eng); //Duration of a single simulation cycle in seconds, is randomly chosen

    while (true) {
        
        system_clock::time_point after = system_clock::now();
        
        auto diff = duration_cast<milliseconds>(after - before);

        if (cycle_duration < diff.count()) {
            _currentPhase = (getCurrentPhase() == TrafficLightPhase::red) ? TrafficLightPhase::green : TrafficLightPhase::red;
            messages.send(std::move(_currentPhase));

            before = system_clock::now();
        }                
        
        int r_sleep = std::rand() % ((10 - 1) + 1) + 1;
        std::this_thread::sleep_for(milliseconds(r_sleep));
    }
}
