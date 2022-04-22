# Simple Multi thread Event Machine


Queues have priority and type (parallel or atomic). Each queue is associated with callback function which will be called for events from this queue

Typical usage:
1) Configure queues
2) Register queues
3) Start event machine with needed number of threads

See example in main.cpp
