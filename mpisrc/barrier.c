#include <stdio.h>
#include <unistd.h>
#include "mpi.h"

typedef enum command {
  PING,
  QUIT
} command;

typedef enum ret_t {
  PONG
} ret_t;

static void master(void) {
  int i;
  double time; 
  command cmd = PING;
  ret_t ret;
  MPI_Request req;
  int flag;
  MPI_Status status;
  int recv;
  for (i = 0; i < 100; i++) {
    recv = 0;
    //MPI_Bcast(&cmd, 1, MPI_INT, 0, MPI_COMM_WORLD);
    //printf("sent broadcast ping\n");
    MPI_Barrier(MPI_COMM_WORLD);
    time = MPI_Wtime();
    while(1) {
      MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, MPI_STATUS_IGNORE);
      if (flag) {
	MPI_Recv(&ret, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        time = MPI_Wtime();
        recv++;
        continue;
      } else if (MPI_Wtime() - time > 5) {
        break;
      }
    }
    printf("Responses from %d nodes\n", recv);
    //sleep(5);
  }
  cmd = QUIT;
  MPI_Bcast(&cmd, 1, MPI_INT, 0, MPI_COMM_WORLD);
}

static void slave(void) {
  command cmd;
  ret_t response = PONG;
  MPI_Status status;
  while(1) {
    MPI_Barrier(MPI_COMM_WORLD);
    //printf("Receiving\n");
    //MPI_Recv(&cmd, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    //MPI_Bcast(&cmd, 1, MPI_INT, 0, MPI_COMM_WORLD);
    //printf("Received\n");
    //if(cmd == PING) {
      //printf("Got ping!\n");
      MPI_Send(&response, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    //} else {
    //  break;
    //} 
  }
}
 
int main( argc, argv )
int  argc;
char **argv;
{
    int rank, size;
    MPI_Init( &argc, &argv );
    MPI_Comm_size( MPI_COMM_WORLD, &size );
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    if (rank == 0)
	master();
    else
        slave();
    MPI_Finalize();
    return 0;
}
