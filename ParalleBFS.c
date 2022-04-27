#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#define SIZE 40

struct queue {
  int items[SIZE];
  int front;
  int rear;
};
int * visited;
struct queue* createQueue();
void enqueue(struct queue* q, int);
int dequeue(struct queue* q);
void display(struct queue* q);
int isEmpty(struct queue* q);
void printQueue(struct queue* q);

struct Graph {
  int numVertices;
  int  **adjMatrix;
  int *degree;
  int * visited;
};

// BFS algorithm
void bfs(struct Graph* graph, int startVertex) {
  struct queue* q = createQueue();

  graph->visited[startVertex] = 1;
  enqueue(q, startVertex);

  while (isEmpty(q)!=1) {
    //printf("\nHI!");
    printQueue(q);
    int currentVertex = dequeue(q);
    printf("Visited %d\n", currentVertex);
    for(int i=0; i<graph->numVertices;i++){
      //isEmpty(q);
      //printf("vertics=%d,i=%d",graph->numVertices,i);
        int temp=graph->adjMatrix[currentVertex][i];
        //printf("\nTemp: %d & Visited Value: %d",temp,graph->visited[temp]);
        if(!graph->visited[temp]){
            enqueue(q,temp);
            graph->visited[temp]=1;
        }
    }
    //printf("BYE!");
  }
}

// Creating a graph
struct Graph* createGraph(int vertices) {
  struct Graph* graph = malloc(sizeof(struct Graph));
  graph->numVertices = vertices;

  graph->adjMatrix = malloc((vertices* sizeof(int))+1);
  graph->visited = malloc((vertices * sizeof(int))+1);
  graph->degree=malloc((vertices * sizeof(int))+1);

  int i;
  for (i = 0; i < vertices; i++) {
    graph->adjMatrix[i] = malloc((vertices * sizeof(int))+1);
    //printf("Visited Creation:%d\n",graph->visited[i]);
    //graph->visited[i] = 0;
  }

  return graph;
}

// Add edge
void addEdge(struct Graph* graph, int vertex,int element) {
  // Add edge from src to dest
  //graph->adjMatrix[vertex][graph->degree[vertex]++]=element;
  //graph->adjMatrix[element][graph->degree[element]++]=vertex;
  graph->adjMatrix[vertex][element]=element;
  graph->adjMatrix[element][vertex]=vertex;
}

// Create a queue
struct queue* createQueue() {
  struct queue* q = malloc(sizeof(struct queue));
  q->front = -1;
  q->rear = -1;
  return q;
}

// Check if the queue is empty
int isEmpty(struct queue* q) {
  if (q->rear == -1)
    return 1;
 return 0;
}

// Adding elements into queue
void enqueue(struct queue* q, int value) {
  if (q->rear == SIZE - 1)
    printf("\nQueue is Full!!");
  else {
    if (q->front == -1)
      q->front = 0;
    q->rear++;
    q->items[q->rear] = value;
  }
}

// Removing elements from queue
int dequeue(struct queue* q) {
  int item;
  if (isEmpty(q)) {
    printf("Queue is empty");
    item = -1;
  } else {
    item = q->items[q->front];
    q->front++;
    if (q->front > q->rear) {
      printf("Resetting queue ");
      q->front = q->rear = -1;
      //printQueue(q);
    }
  }
  //printf("ITEM%d",item);
  return item;
}

// Print the queue
void printQueue(struct queue* q) {
  int i = q->front;

  if (isEmpty(q)) {
    printf("Queue is empty");
  } else {
    printf("\nQueue contains \n");
    for (i = q->front; i < q->rear + 1; i++) {
      printf("%d ", q->items[i]);
    }
  }
}
int main(int argc, char ** argv){
  int rank, world_size;
  MPI_Init( &argc ,  &argv);
  MPI_Comm_rank( MPI_COMM_WORLD , &rank);
  MPI_Comm_size( MPI_COMM_WOLRD , &world_size);
  struct Graph * graph;
  struct Queue * queue;
  int num_vertcies;
  int source_vertex;
  int * receive_row;
  int * bfs_result;
  if(rank==0){
    num_vertcies=6;
    graph=createGraph(num_vertcies);
    bfs_result=malloc(sizeof(int)*num_vertcies);
    queue=createQueue();
    addEdge(graph, 0, 1);
    addEdge(graph, 0, 2);
    addEdge(graph, 1, 2);
    addEdge(graph, 1, 4);
    addEdge(graph, 1, 3);
    addEdge(graph, 2, 4);
    addEdge(graph, 3, 4);
    for(int i=0; i<num_vertcies;i++){
    MPI_Send(graph->adjMatrix[i] , num_vertcies , MPI_INT , receive_row , 0 , MPI_COMM_WORLD);
  }
  for(int j=0; j<world_size;j++){
    MPI_Recv( &bfs_result[j] , 1 , MPI_INT, j , 2 , MPI_COMM_WORLD , MPI_STATUS_IGNORE);
  }
    source_vertex=0;
  }
  MPI_Bcast( &num_vertcies , 1 , MPI_INT , 0 , MPI_COMM_WORLD);
  MPI_Bcast( &source_vertex , 1 , MPI_INT , 0 , MPI_COMM_WORLD);
  visited=calloc(sizeof(int),num_vertcies);
  if(rank!=0){
    int index=0;
    queue=createQueue();
    for(int i=0; i<num_vertcies;i++){
      MPI_Recv( receive_row , num_vertcies , MPI_INT , 0 , 0 , MPI_COMM_WORLD , MPI_STATUS_IGNORE);
      for(int j=0;j<num_vertcies;j++){
        int temp=receive_row[i];
        if(!visited[temp]){
          visited[temp]=1;
          MPI_Send( &temp , 1 , MPI_INT , 0 , 2 , MPI_COMM_WORLD);
        }
      }
    }
  }
  //MPI_Scatter( graph->adjMatrix , num_vertcies , MPI_INT , receive_row , num_vertcies , MPI_Datatype recvtype , int root , MPI_Comm comm);
  MPI_Finalize();
  return 0;
}