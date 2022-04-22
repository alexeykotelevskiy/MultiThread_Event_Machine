#include "EmCore.hpp"

#include <iostream>

EmQueueId queue1, queue2;

void handler1(EmEventId eventId)
{
    auto& em = EventMachine::instance();
    std::cout << "handler1 " << eventId << std::endl;
    em.emEventSend(10, queue2);
}

void handler2(EmEventId eventId)
{
    std::cout << "handler2 " << eventId << std::endl;
}

void globalStartHandler()
{
    auto& em = EventMachine::instance();
    std::cout << "globalStartHandler" << std::endl;
    em.emEventSend(0, queue2);
    em.emEventSend(1, queue1);
    em.emEventSend(2, queue2);
    em.emEventSend(3, queue1);
}

int main()
{
    auto& em = EventMachine::instance();
    EmQueueConfig config1 = {EmQueueType::parallel, 200 , &handler1};
    EmQueueConfig config2 = {EmQueueType::parallel, 100 , &handler2};

    queue1 = em.registerEmQueue(config1);
    queue2 = em.registerEmQueue(config2);
    em.registerGlobalStartHandler(&globalStartHandler);
    em.start(4);
    return 0;
}