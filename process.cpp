#include "process.hpp"

void *comThread(void *ptr)
{
    MPI_Status status;
    packet_t recvPkt;
    packet_t sendPkt;
    
    while(true)
    {
        recvPacket(recvPkt, status);

        std::string actionStr = recvPkt.replicaAction == ReplicaAction::READ ? "reading" : "writing";

        switch (status.MPI_TAG)
        {
            case REQ:
            {
                pthread_mutex_lock(&reqMutex);
                if (PRINT_GOT_REQ)
                    print("[%d] [t%d] Got REQ from process %d to access doc %d for %s", rank, recvPkt.timestamp+1, recvPkt.source, recvPkt.docId, actionStr.c_str());
                if (inReleaseList(recvPkt.docId))
                {
                    // leaving replica - sending release messages
                    sendPkt.docId = recvPkt.docId;
                    sendPkt.replicaAction = -1; // doesn't matter, there could be anything
                    sendPacketToAll(sendPkt, RELEASE);
                    
                    print("[%d] [t%d] Sending RELEASE to all - releasing doc %d replica", rank, sendPkt.timestamp, sendPkt.docId);

                    // removing docId from docsToRelease list
                    removeFromReleaseList(recvPkt.docId);
                }
                pthread_mutex_unlock(&reqMutex);
                
                // adding REQ to specific doc queue
                addToQueue(recvPkt);

                sendPkt.docId = recvPkt.docId;
                sendPkt.replicaAction = recvPkt.replicaAction;
                if (recvPkt.source != rank)
                    sendPacket(sendPkt, recvPkt.source, ACK);

                break;
            }
            case ACK:
            {
                if (PRINT_GOT_ACK)
                    print("[%d] [t%d] Got ACK from process %d to access doc %d for %s", rank, recvPkt.timestamp+1, recvPkt.source, recvPkt.docId, actionStr.c_str());
                incAcksCounter();
                updateTimestamp(recvPkt.timestamp);
                
                if (acksCounter == N - 1 && canUse(currentReq))
                    sendSignal(); // cond signal to main thread

                break;
            }
            case RELEASE:
            {
                if (PRINT_GOT_RELEASE)
                    print("[%d] [t%d] Got RELEASE from process %d about accessing doc %d for %s", rank, recvPkt.timestamp+1, recvPkt.source, recvPkt.docId, actionStr.c_str());
                removeFromQueue(recvPkt);
                updateTimestamp(recvPkt.timestamp);

                if (recvPkt.docId == currentReq.docId && canUse(currentReq))
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

        resetAcksCounter();
        
        sendPacketToAll(sendPkt, REQ);

        currentReq = sendPkt;

        print("[%d] [t%d] Sending REQs to all to access doc %d replica for %s", rank, sendPkt.timestamp, docId, actionStr.c_str());

        // collecting acks and waiting for replica access - proper cond signal will be send to wake this thread
        waitingForOwnTurn();

        // using replica
        print("[%d] [t%d] Doc %d replica: %s", rank, timestamp, docId, actionStr.c_str());
        delay(actionStr[0]);

        // releasing only after getting req for this doc

        // checking if we collected same doc reqs already
        pthread_mutex_lock(&reqMutex);
        if (areSameReqsPresent(sendPkt))
        {
            // leaving replica - sending release messages
            sendPacketToAll(sendPkt, RELEASE);
            print("[%d] [t%d] Sending RELEASE to all - releasing doc %d replica", rank, sendPkt.timestamp, sendPkt.docId);
        }
        else
        {
            addToReleaseList(docId);
        }
        pthread_mutex_unlock(&reqMutex);
        print("[%d] [t%d] Done for now, going to sleep", rank, timestamp);
        delay('e');
    }
}