#pragma once

#include <atomic>
#include <vector>
#include <condition_variable>
#include <functional>
#include "EmHandler.hpp"
#include "EmQueue.hpp"
#include "EmEvent.hpp"
#include <queue>
#include <map>
#include <thread>

//There is main class for event machine. It's singleton. To get the object ref use
//auto& em = EventMachine::instance();
//Queues should be configured before calling em.start()
class EventMachine
{
public:
    //registers new em queue.
    EmQueueId registerEmQueue(const EmQueueConfig& config);
    //callback will be invoked right after starting event machine
    void registerGlobalStartHandler(std::function<void()> handler);
    //sends event with eventId to queue with queueId
    void emEventSend(EmEventId eventId, EmQueueId queueId);
    //starts event machine processing, runs numOfThread threads to process events. Blocks current thread
    void start(uint8_t numOfThreads);
    //stops event machine. Unblocks thread blocked by start
    void stop();

    EventMachine(const EventMachine&) = delete;
    EventMachine& operator=(const EventMachine &) = delete;
    EventMachine(EventMachine &&) = delete;
    EventMachine & operator=(EventMachine &&) = delete;

    static auto& instance(){
        static EventMachine eventMachine;
        return eventMachine;
    }
private:
    EventMachine() = default;
    void threadWorker();
    void clearResources();

    size_t totalNumOfEvents{0};
    std::atomic<bool> exitFlag{false};
    std::map<EmQueueKey, EmQueue> emQueues;
    std::vector<EmQueueConfig> emQueueConfigs;
    std::vector<std::thread> threads;
    std::function<void()> globalStartHandler;
    std::condition_variable condVar;
    std::mutex m;
};
