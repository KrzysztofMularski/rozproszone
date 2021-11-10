#include "main.hpp"
#include "process.hpp"

void *comThread(void *ptr)
{
    MPI_Status status;
    packet_t recv;
    
    while(true)
    {
        recvPacket(recv, status);

        switch (status.MPI_TAG)
        {
            case REQ:
            {

                break;
            }
            case ACK:
            {

                break;
            }
            case RELEASE:
            {

                break;
            }
        }
    }
}

void mainLoop()
{
    packet_t pkt;
    while(true)
    {
        // sending req
        int docId = rand() % D;
        ReplicaAction rAction;
        if (rand() % 100 < PROB)
            rAction = ReplicaAction::WRITE;
        else
            rAction = ReplicaAction::READ;
        pkt = {
            docId,
            rAction,
            rank,
            timestamp + 1
        };
        sendPacketToAll(pkt, REQ);
        
    }
}