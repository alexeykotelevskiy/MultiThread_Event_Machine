#include "EmCore.hpp"

#include <iostream>
#include <atomic>
#include <thread>
#include <cassert>
#include <mutex>
std::atomic<size_t> numOfReceivedEvents{0};
size_t numOfExpectedEvents{0};
EmQueueId queue1, queue2, syncQueue;
std::mutex testMutex;
using namespace std::chrono_literals;


void handler1(EmEventId eventId)
{
    auto& em = EventMachine::instance();
    std::this_thread::sleep_for(100ms);
    em.emEventSend(1, syncQueue);
}

void atomicHandler(EmEventId eventId)
{
    auto& em = EventMachine::instance();
    assert(testMutex.try_lock());
    std::this_thread::sleep_for(50ms);
    testMutex.unlock();
    em.emEventSend(1, syncQueue); 
}

void handler2(EmEventId eventId)
{
    auto& em = EventMachine::instance();
    std::this_thread::sleep_for(100ms);
    em.emEventSend(1, syncQueue);
}

void syncHandler(EmEventId eventId)
{
    numOfReceivedEvents++;
    if (numOfReceivedEvents.load() == numOfExpectedEvents)
    {
        auto& em = EventMachine::instance();
        em.stop();
    }
}

//tests 2 parallel queues
void test1()
{
    auto& em = EventMachine::instance();
    EmQueueConfig config1 = {EmQueueType::parallel, 200 , &handler1};
    EmQueueConfig config2 = {EmQueueType::parallel, 100 , &handler2};
    EmQueueConfig syncConfig{EmQueueType::parallel, 0, &syncHandler};

    queue1 = em.registerEmQueue(config1);
    queue2 = em.registerEmQueue(config2);
    syncQueue = em.registerEmQueue(syncConfig);
    em.emEventSend(1, queue1);
    em.emEventSend(2, queue2);
    em.emEventSend(3, queue1);
    em.emEventSend(4, queue2);
    numOfExpectedEvents = 4;
    em.start(4);
    std::this_thread::sleep_for(500ms);
    assert(numOfReceivedEvents == numOfExpectedEvents);
    std::cout << "TEST1: All events were processed" << std::endl;
}

//test one atomic queue
void test2()
{
    auto& em = EventMachine::instance();
    EmQueueConfig config1 = {EmQueueType::atomic, 200 , &atomicHandler};
    EmQueueConfig syncConfig{EmQueueType::parallel, 0, &syncHandler};

    queue1 = em.registerEmQueue(config1);
    syncQueue = em.registerEmQueue(syncConfig);
    const size_t numOfEvents = 100;
    for (int i = 0; i < numOfEvents; i++)
    {
        em.emEventSend(i, queue1);
    }
    numOfExpectedEvents = numOfEvents;
    em.start(4);
    std::this_thread::sleep_for(500ms);
    assert(numOfReceivedEvents == numOfExpectedEvents);
    std::cout << "TEST2: All events were processed" << std::endl;
}

int main()
{
    test1();
    test2();
    return 0;
}