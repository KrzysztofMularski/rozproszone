#include "main.hpp"
#include "process.hpp"

void *comThread(void *ptr)
{
    printf("hello com\n");
}

void mainLoop()
{
    printf("hello main loop\n");
    // while(1)
    // {
    //     switch()
    // }
}