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


class EventMachine
{
public:
    EmQueueId registerEmQueue(const EmQueueConfig& config);
    void registerGlobalStartHandler(std::function<void()> handler);
    void emEventSend(EmEventId eventId, EmQueueId queueId);
    void start(uint8_t numOfThreads);

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
    size_t totalNumOfEvents{0};
    std::map<EmQueueKey, EmQueue> emQueues;
    std::vector<EmQueueConfig> emQueueConfigs;
    std::function<void()> globalStartHandler;
    std::condition_variable condVar;
    std::mutex m;
};
