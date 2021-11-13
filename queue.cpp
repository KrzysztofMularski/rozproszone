#include "queue.hpp"

void printQueue(const int& docId)
{
    std::cout << "Queue:\n";
    for (const queuePosition& qPos : docsQueues[docId])
    {
        std::cout << qPos.timestamp << ", " << qPos.processId << ", " << qPos.replicaAction << std::endl;
    }
}

void addToQueue(const packet_t& pkt)
{
    queuePosition qPos;
    qPos.timestamp = pkt.timestamp;
    qPos.processId = pkt.source;
    qPos.replicaAction = pkt.replicaAction;

    std::list<queuePosition>& queue = docsQueues[pkt.docId];

    if (queue.empty())
    {
        queue.insert(queue.begin(), qPos);
        return;
    }

    std::list<queuePosition>::iterator it;
    for (it = queue.begin(); it != queue.end(); ++it)
    {
        if ((pkt.timestamp < it->timestamp) || (pkt.timestamp == it->timestamp && pkt.source < it->processId))
        {
            queue.insert(it, qPos);
            return;
        }
    }
    queue.insert(it, qPos);
}

void removeFromQueue(const packet_t& pkt)
{
    std::list<queuePosition>& queue = docsQueues[pkt.docId];

    if (queue.empty())
        return;

    std::list<queuePosition>::iterator it;
    for (it = queue.begin(); it != queue.end(); ++it)
    {
        if (pkt.timestamp >= it->timestamp && pkt.source == it->processId) // replicaAction doesn't matter !
        {
            queue.erase(it);
            return;
        }
    }
}

bool canUse(const packet_t& pkt)
{
    if (pkt.replicaAction == ReplicaAction::READ)
        return canRead(pkt);
    else if (pkt.replicaAction == ReplicaAction::WRITE)
        return canWrite(pkt);
}

bool canRead(const packet_t& pkt)
{
    std::list<queuePosition>& queue = docsQueues[pkt.docId];
    std::list<queuePosition>::iterator it;

    int readersCounter = 0;
    int writersCounter = 0;
    
    for (it = queue.begin(); it != queue.end(); ++it)
    {
        if (it->timestamp == pkt.timestamp && it->processId == pkt.source && it->replicaAction == pkt.replicaAction)
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

bool canWrite(const packet_t& pkt)
{
    std::list<queuePosition>::iterator it = docsQueues[pkt.docId].begin();

    return (it->timestamp == pkt.timestamp && it->processId == pkt.source && it->replicaAction == pkt.replicaAction);
}

bool areSameReqsPresent(const packet_t& pkt)
{
    std::list<queuePosition>& queue = docsQueues[pkt.docId];
    std::list<queuePosition>::iterator it = queue.begin();

    for (it = queue.begin(); it != queue.end(); ++it)
    {
        return (
            it->timestamp == pkt.timestamp &&
            it->processId == pkt.source &&
            it->replicaAction == pkt.replicaAction &&
            it != queue.end()
        );
    }
}