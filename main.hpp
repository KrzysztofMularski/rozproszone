#pragma once

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string>
#include <iostream>
#include <unistd.h>
#include <list>

#include "process.hpp"
#include "printer.hpp"

#define N 5 // processes
#define K 4 // replicas
#define D 2 // documents

#define PROB 100 // probability of writing to replica [0-100%]

#define REQ 0
#define ACK 1
#define RELEASE 2

// sleeping times [s]
#define SLEEP_TIME_READING 3
#define SLEEP_TIME_WRITING 5
#define SLEEP_TIME_END_CYCLE 5
#define SLEEP_TIME_RANDOM_FLUCTUATIONS 1

enum ReplicaAction {
    READ,
    WRITE
};

struct packet_t {
    int timestamp;
    int replicaAction;
    int docId;
    int source;
};

struct queuePosition {
    int timestamp;
    int processId;
    int replicaAction;
};

extern MPI_Datatype MPI_PACKET_T;

extern int rank;
extern int size;
extern int colorCode;

extern int timestamp;

extern std::list<queuePosition> docsQueues[D];

extern int acksCounter;

extern packet_t currentReq;

extern pthread_mutex_t printerMutex;
extern pthread_mutex_t reqMutex;

void updateTimestamp(int);

void recvPacket(packet_t&, MPI_Status&);

void sendPacket(packet_t&, int&, int);

void sendPacketToAll(packet_t&, int);

void waitingForOwnTurn();

void delay(const char&);

void incAcksCounter();

void resetAcksCounter();

void sendSignal();

void addToReleaseList(const int&);

void removeFromReleaseList(const int&);

bool inReleaseList(const int&);
