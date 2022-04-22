#pragma once

#include <cstdint>
#include <queue>
#include <atomic>
#include "EmEvent.hpp"
using EmQueueId = uint32_t;

enum EmQueueType
{
    atomic, //new event will start only after the previous one has been completed
    parallel //events are processed in parallel
};

struct EmQueueConfig
{
    EmQueueType type; // type of the queue
    uint32_t      priority; //priority of the queue. Lower values means high priority
    std::function<void(EmEventId)> handler; //callback will be called for events from this queue
};

struct EmQueueKey
{
    uint32_t  priority;
    EmQueueId queueId;
    EmQueueKey() = default;
    EmQueueKey(uint32_t priority_, EmQueueId queueId_) : priority(priority_), queueId(queueId_) {} 
};

static bool operator<(const EmQueueKey& lhs, const EmQueueKey& rhs)
{
    if (lhs.priority < rhs.priority) return true;
    if (lhs.priority == rhs.priority) return lhs.queueId < rhs.queueId;
    return false;
}

struct EmQueue
{
    enum EmQueueStatus
    {
        open,
        blocked
    };
    EmQueue() = default;
    EmQueue(const EmQueue& x) : queueKey(x.queueKey), queue(x.queue), queueStatus(x.queueStatus.load()), mutex() {}
    EmQueue& operator=(const EmQueue& x)
    {
        queueKey = x.queueKey;
        queue = x.queue;
        queueStatus.store(x.queueStatus.load());
        return *this;
    }

    EmQueueKey queueKey;
    mutable std::queue<EmEventId> queue;
    mutable std::atomic<EmQueueStatus> queueStatus;
    mutable std::mutex mutex;
};
