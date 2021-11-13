#include "process.hpp"

void *comThread(void *ptr)
{
    MPI_Status status;
    packet_t recvPkt;
    packet_t sendPkt;
    
    while(true)
    {
        recvPacket(recvPkt, status);

        switch (status.MPI_TAG)
        {
            case REQ:
            {
                // adding REQ to specific doc queue
                addToQueue(recvPkt);

                sendPkt.docId = recvPkt.docId;
                sendPkt.replicaAction = recvPkt.replicaAction;
                sendPacket(sendPkt, recvPkt.source, ACK);

                // std::string actionStr = recvPkt.replicaAction == ReplicaAction::READ ? "reading" : "writing";
                // print("Got REQ with ts=%d from process %d to access doc %d (%s), sending ACK", recvPkt.timestamp, recvPkt.source, recvPkt.docId, actionStr.c_str());

                break;
            }
            case ACK:
            {
                if (++acksCounter == N - 1 &&
                    (
                        (currentReq.replicaAction == ReplicaAction::READ && canRead(currentReq)) ||
                        (currentReq.replicaAction == ReplicaAction::WRITE && canWrite(currentReq))
                    )
                )
                sendSignal(); // cond signal to main thread

                break;
            }
            case RELEASE:
            {
                removeFromQueue(recvPkt);

                if (recvPkt.docId == currentReq.docId &&
                    (
                        (currentReq.replicaAction == ReplicaAction::READ && canRead(currentReq)) ||
                        (currentReq.replicaAction == ReplicaAction::WRITE && canWrite(currentReq))
                    )
                )
                sendSignal(); // cond signal to main thread

                break;
            }
        }
    }
}

void mainLoop()
{
    packet_t sendPkt;
    while(true)
    {
        // sending req
        int docId = rand() % D;
        ReplicaAction replicaAction;
        std::string actionStr;
        if (rand() % 100 >= PROB)
        {
            replicaAction = ReplicaAction::READ;
            actionStr = "reading";
        }
        else
        {
            replicaAction = ReplicaAction::WRITE;
            actionStr = "writing";
        }
        sendPkt.docId = docId;
        sendPkt.replicaAction = replicaAction;

        acksCounter = 0;
        
        sendPacketToAll(sendPkt, REQ);

        currentReq = sendPkt;

        print("Sending REQs to all to access doc %d replica for %s", docId, actionStr.c_str());

        // collecting acks and waiting for replica access - proper cond signal will be send to wake this thread
        waitingForOwnTurn();

        // using replica
        print("Doc %d replica: %s", docId, actionStr.c_str());
        delay(actionStr[0]);

        // todo: releasing only after getting req for this doc
        // leaving replica - sending release messages
        sendPacketToAll(sendPkt, RELEASE);

        print("Done for now, going to sleep, sending RELEASE");
        delay('e');
    }
}