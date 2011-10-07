#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "mpi.h"

typedef enum command {
  PING,
  QUIT
} command;

typedef enum ret_t {
  PONG
} ret_t;


static void master(void) {
  int i,j;
  double time; 
  command cmd = PING;
  ret_t ret;
  MPI_Request req;
  int flag;
  MPI_Status status;
  int recv;
  char buf[80];
  int size, rank;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  int *recvd = malloc(sizeof(int) * size);
  FILE *fd = fopen("/root/log", "w+");
  if(fd == NULL) {
    printf("could not open file\n");
  }
/*   MPI_Comm graph; */
/*   int *index = malloc(size * sizeof(int)); */
/*   int *edges = malloc((2*size + 2) * sizeof(int)); */
/*   for (i = 0; i < size; i++) { */
/*     index[i] = (size - 1 + i); */
/*   } */
/*   for (i = 0; i < size-1; i++) { */
/*     edges[i] = i+1; */
/*   } */
/*   for (i = size-1; i < (2 * size + 2); i++) { */
/*     edges[i] = 0; */
/*   } */
/*   MPI_Graph_create(MPI_COMM_WORLD, size, index, edges, 0, &graph); */
/*   printf("graph created\n"); */
/*   MPI_Barrier(MPI_COMM_WORLD); */
/*   printf("starting\n"); */
  for (i = 0; i < 100; i++) {
    memset(recvd, 0, sizeof(int)*size);
    recvd[rank] = 1;
    recv = 0;
    MPI_Bcast(&cmd, 1, MPI_INT, 0, MPI_COMM_WORLD);
    time = MPI_Wtime();
    while(1) {
      MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, MPI_STATUS_IGNORE);
      if (flag) {
	MPI_Recv(&ret, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        time = MPI_Wtime();
        recv++;
        recvd[status.MPI_SOURCE] = 1;
        continue;
      } else if (MPI_Wtime() - time > 5) {
        break;
      }
    }
    for(j = 0; j < size; j++) {
      fprintf(fd, "%d\n", recvd[j]);
    }
    recv++; 
    printf("%d nodes responded\n", recv);
    fprintf(fd, "total:%d\n", recv);
    fflush(fd);
  }
  cmd = QUIT;
  MPI_Bcast(&cmd, 1, MPI_INT, 0, MPI_COMM_WORLD);
  fclose(fd);
}

static void slave(void) {
  command cmd;
  ret_t response = PONG;
  MPI_Status status;
/*   int size, rank; */
/*   MPI_Comm_size(MPI_COMM_WORLD, &size); */
/*   MPI_Comm_rank(MPI_COMM_WORLD, &rank); */
/*   int i; */
/*   MPI_Comm graph; */
/*   int *index = malloc(size * sizeof(int)); */
/*   int *edges = malloc((2*size + 2) * sizeof(int)); */
/*   for (i = 0; i < size; i++) { */
/*     index[i] = (size - 1 + i); */
/*   } */
/*   for (i = 0; i < size-1; i++) { */
/*     edges[i] = i+1; */
/*   } */
/*   for (i = size-1; i < (2 * size + 2); i++) { */
/*     edges[i] = 0; */
/*   } */
/*   MPI_Graph_create(MPI_COMM_WORLD, size, index, edges, 0, &graph); */
/*   MPI_Barrier(MPI_COMM_WORLD); */
  while(1) {
    MPI_Bcast(&cmd, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if(cmd == PING) {
      MPI_Send(&response, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    } else {
      break;
    } 
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
