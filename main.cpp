#include "main.hpp"

MPI_Datatype MPI_PACKET_T;

pthread_t threadCom;
int rank;
int size;
int colorCode;

int timestamp;

pthread_mutex_t timestampMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t waitingForOwnTurnMutex = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t waitingForOwnTurnCond = PTHREAD_COND_INITIALIZER;

std::list<queuePosition> docsQueues[D];

int acksCounter;

packet_t currentReq = {-1, -1, -1, -1};

void check_thread_support(int provided)
{
    // printf("THREAD SUPPORT: chcemy %d. Co otrzymamy?\n", provided);
    switch (provided) {
        case MPI_THREAD_SINGLE: 
            printf("Brak wsparcia dla wątków, kończę\n");
            /* Nie ma co, trzeba wychodzić */
	    fprintf(stderr, "Brak wystarczającego wsparcia dla wątków - wychodzę!\n");
	    MPI_Finalize();
	    exit(-1);
	    break;
        case MPI_THREAD_FUNNELED: 
            printf("tylko te wątki, ktore wykonaly mpi_init_thread mogą wykonać wołania do biblioteki mpi\n");
	    break;
        case MPI_THREAD_SERIALIZED: 
            /* Potrzebne zamki wokół wywołań biblioteki MPI */
            printf("tylko jeden watek naraz może wykonać wołania do biblioteki MPI\n");
	    break;
        case MPI_THREAD_MULTIPLE:
            //printf("Pełne wsparcie dla wątków\n"); /* tego chcemy. Wszystkie inne powodują problemy */
	    break;
        default: printf("Nikt nic nie wie\n");
    }
}

void init(int *argc, char ***argv)
{
    int provided;
    MPI_Init_thread(argc, argv, MPI_THREAD_MULTIPLE, &provided);
    check_thread_support(provided);

    const int nitems = 4;
    int blocklengths[4] = {1, 1, 1, 1};
    MPI_Datatype types[4] = {MPI_INT, MPI_INT, MPI_INT, MPI_INT};

    MPI_Aint    offsets[4];
    offsets[0] = offsetof(packet_t, timestamp);
    offsets[1] = offsetof(packet_t, replicaAction);
    offsets[2] = offsetof(packet_t, docId);
    offsets[3] = offsetof(packet_t, source);

    MPI_Type_create_struct(nitems, blocklengths, offsets, types, &MPI_PACKET_T);
    MPI_Type_commit(&MPI_PACKET_T);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    timestamp = 0;
    colorCode = rank % 7 + 31;

    print("I'm process %d", rank);
    
    srand(rank);

    pthread_create( &threadCom, NULL, comThread, 0 );
}

void finalize()
{	
    MPI_Type_free(&MPI_PACKET_T);
    MPI_Finalize();
}

void updateTimestamp(int newTs = -1)
{
    pthread_mutex_lock(&timestampMutex);
    if (timestamp > newTs)
        ++timestamp;
    else
        timestamp = newTs + 1;
    pthread_mutex_unlock(&timestampMutex);
}

void recvPacket(packet_t& pkt, MPI_Status& status)
{
	MPI_Recv(&pkt, 1, MPI_PACKET_T, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    updateTimestamp(pkt.timestamp);
}

void sendPacket(packet_t& pkt, int& destination, int tag)
{
    updateTimestamp();
    pkt.source = rank;
    pkt.timestamp = timestamp;
    MPI_Send(&pkt, 1, MPI_PACKET_T, destination, tag, MPI_COMM_WORLD);
}

void sendPacketToAll(packet_t& pkt, int tag)
{
    updateTimestamp();
    pkt.source = rank;
    pkt.timestamp = timestamp;
    for (int i=0; i<size; ++i)
        MPI_Send(&pkt, 1, MPI_PACKET_T, i, tag, MPI_COMM_WORLD);
}

void waitingForOwnTurn()
{
    pthread_mutex_lock(&waitingForOwnTurnMutex);
    pthread_cond_wait(&waitingForOwnTurnCond, &waitingForOwnTurnMutex);
    pthread_mutex_unlock(&waitingForOwnTurnMutex);
}

void delay(const char& action)
{
    int actionTime;

    if (action == 'r')
        actionTime = SLEEP_TIME_READING;
    else if (action == 'w')
        actionTime = SLEEP_TIME_WRITING;
    else if (action == 'e')
        actionTime = SLEEP_TIME_END_CYCLE;

    const int sleepTime = actionTime * 1000 + rand() % (SLEEP_TIME_RANDOM_FLUCTUATIONS * 2000) - 1000; // ms
    usleep(sleepTime*1000);
}

void sendSignal()
{
    pthread_cond_signal(&waitingForOwnTurnCond);
}

int main(int argc, char **argv)
{
    init(&argc, &argv);

    mainLoop();

    finalize();

    return 0;
}