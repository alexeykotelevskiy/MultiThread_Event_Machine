#include "EmCore.hpp"
#include <thread>
#include <memory>
#include <mutex>
#include <iostream>
EmQueueId EventMachine::registerEmQueue(const EmQueueConfig& config)
{
    emQueueConfigs.push_back(config);
    const EmQueueId queueId = emQueueConfigs.size() - 1;
    EmQueue emQueue;
    emQueue.queueKey.queueId = queueId;
    emQueue.queueKey.priority = config.priority;
    emQueue.queueStatus.store(EmQueue::EmQueueStatus::open);
    emQueues[EmQueueKey(config.priority, queueId)] = emQueue;
    return queueId;
}

void EventMachine::emEventSend(EmEventId eventId, EmQueueId queueId)
{
    auto search = emQueues.find(EmQueueKey(emQueueConfigs[queueId].priority, queueId));
    {
        std::scoped_lock lk(m, search->second.mutex);
        totalNumOfEvents++;
        search->second.queue.push(eventId);
    }
    condVar.notify_one();
}


void EventMachine::threadWorker()
{
    while (true)
    {
        bool isProcessNeeded = false;
        EmEventId event;
        std::unique_lock lk(m);
        condVar.wait(lk, [this] {return totalNumOfEvents > 0 || exitFlag.load();});
        if (exitFlag.load())
        {
            break;
        }
        totalNumOfEvents--;
        lk.unlock();
        for (auto& queuePairObj : emQueues)
        {
            auto& queueObj = queuePairObj.second;
            std::unique_lock queueLock(queueObj.mutex);
            if (queueObj.queue.empty() || queueObj.queueStatus.load() == EmQueue::EmQueueStatus::blocked)
            {
                continue;
            }
            event = queueObj.queue.front();
            queueObj.queue.pop();
            const EmQueueConfig& queueConfig = emQueueConfigs[queueObj.queueKey.queueId];
            if (queueConfig.type == EmQueueType::atomic)
            {
                queueObj.queueStatus.store(EmQueue::EmQueueStatus::blocked);
            }
            queueLock.unlock();
            isProcessNeeded = true;
            queueConfig.handler(event);
            if (queueConfig.type == EmQueueType::atomic)
            {
                queueObj.queueStatus.store(EmQueue::EmQueueStatus::open);
                condVar.notify_one();
            }
            break;
        }
        if (!isProcessNeeded)
        {
            lk.lock();
            totalNumOfEvents++;
        }
    }
}

void EventMachine::registerGlobalStartHandler(std::function<void()> handler)
{
    globalStartHandler = handler;
}

void EventMachine::start(uint8_t numOfThreads)
{
    exitFlag.store(false);
    for (int i = 0; i < numOfThreads; i++)
    {

        threads.emplace_back(&EventMachine::threadWorker, this);
    }
    if (globalStartHandler != nullptr)
    {
        globalStartHandler();
    }
    for (auto& thread : threads)
    {
        thread.join();
    }
    clearResources();
}

void EventMachine::clearResources()
{
    totalNumOfEvents = 0;
    emQueues.clear();
    emQueueConfigs.clear();
    threads.clear();
    globalStartHandler = nullptr;
}

void EventMachine::stop()
{
    exitFlag.store(true);
    condVar.notify_all();
}
