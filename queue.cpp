#include "queue.hpp"

void addToQueue(packet_t& currentReq)
{
    queuePosition qPos;
    qPos.timestamp = currentReq.timestamp;
    qPos.processId = currentReq.source;
    qPos.replicaAction = currentReq.replicaAction;

    std::list<queuePosition>& queue = docsQueues[currentReq.docId];

    if (queue.empty())
    {
        queue.insert(queue.begin(), qPos);
        return;
    }

    std::list<queuePosition>::iterator it;
    for (it = queue.begin(); it != queue.end(); ++it)
    {
        if ((currentReq.timestamp < it->timestamp) || (currentReq.timestamp == it->timestamp && currentReq.source < it->processId))
        {
            queue.insert(it, qPos);
            return;
        }
    }
    queue.insert(it, qPos);
}

void removeFromQueue(packet_t& currentReq)
{
    std::list<queuePosition>& queue = docsQueues[currentReq.docId];

    if (queue.empty())
        return;

    std::list<queuePosition>::iterator it;
    for (it = queue.begin(); it != queue.end(); ++it)
    {
        if (currentReq.timestamp == it->timestamp && currentReq.source == it->processId && currentReq.replicaAction == it->replicaAction)
        {
            queue.erase(it);
            return;
        }
    }
}

bool canRead(packet_t& currentReq)
{
    std::list<queuePosition>& queue = docsQueues[currentReq.docId];
    std::list<queuePosition>::iterator it;

    int readersCounter = 0;
    int writersCounter = 0;
    
    for (it = queue.begin(); it != queue.end(); ++it)
    {
        if (it->timestamp == currentReq.timestamp && it->processId == currentReq.source && it->replicaAction == currentReq.replicaAction)
        {
            if (writersCounter > 0)
                return false;
            else if (readersCounter < K)
                return true;
            return false;
        }
        if (it->replicaAction == ReplicaAction::READ)
            ++readersCounter;
        else
            ++writersCounter;
    }
}

bool canWrite(packet_t& currentReq)
{
    std::list<queuePosition>::iterator it = docsQueues[currentReq.docId].begin();

    return (it->timestamp == currentReq.timestamp && it->processId == currentReq.source && it->replicaAction == currentReq.replicaAction);
}