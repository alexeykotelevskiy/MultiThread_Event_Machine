# Simple Multi thread Event Machine


Queues have priority and type (parallel or atomic). Each queue is associated with callback function which will be called for events from this queue

Typical usage:
1. Configure queues
2. Register queues
3. Start event machine with needed number of threads

example:
```cpp
void handler1(EmEventId eventId)
{
    ....
    em.emEventSend(eventId, queue2);
    ...
}

void handler2(EmEventId eventId)
{
    ....
}

...

int main()
{
    auto& em = EventMachine::instance();
    EmQueueConfig config1 = {EmQueueType::parallel, 200 , &handler1};
    EmQueueConfig config2 = {EmQueueType::parallel, 100 , &handler2};

    queue1 = em.registerEmQueue(config1);
    queue2 = em.registerEmQueue(config2);
    em.start(4);  
}
```
See example in main.cpp
