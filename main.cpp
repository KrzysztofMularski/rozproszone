#include "main.hpp"
#include "process.hpp"
#include <pthread.h>

pthread_t threadCom;

void check_thread_support(int provided)
{
    //printf("THREAD SUPPORT: chcemy %d. Co otrzymamy?\n", provided);
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
        case MPI_THREAD_MULTIPLE: //printf("Pełne wsparcie dla wątków\n"); /* tego chcemy. Wszystkie inne powodują problemy */
	    break;
        default: printf("Nikt nic nie wie\n");
    }
}

void init(int *argc, char ***argv)
{
    int provided;
    MPI_Init_thread(argc, argv, MPI_THREAD_MULTIPLE, &provided);
    check_thread_support(provided);

    pthread_create( &threadCom, NULL, comThread, 0 );
    // printf("I'm process %d", rank);

}

void finalize()
{	
    //printf("czekam na wątek \"komunikacyjny\"\n");
    // pthread_join(threadComm, NULL);
    //if (rank == 0) pthread_join(threadMon,NULL);
    // MPI_Type_free(&MPI_PACKET_T);
    MPI_Finalize();
}

int main(int argc, char **argv)
{
    init(&argc, &argv);


    finalize();

    return 0;
}