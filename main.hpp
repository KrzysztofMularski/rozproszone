#pragma once

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "process.hpp"

#define N 5 // processes
#define K 3 // replicas
#define D 2 // documents

#define PROB 25 // probability of writing to replica

#define REQ 0
#define ACK 1
#define RELEASE 2

enum ReplicaAction {
    READ,
    WRITE
};

typedef struct {
    int timestamp;
    int replicaAction;
    int docId;
    int source;
} packet_t;

extern MPI_Datatype MPI_PACKET_T;

extern int rank;
extern int size;

extern int timestamp;

void updateTimestamp(int);

void recvPacket(packet_t&, MPI_Status&);

void sendPacket(packet_t&, int&, int);

void sendPacketToAll(packet_t&, int);

