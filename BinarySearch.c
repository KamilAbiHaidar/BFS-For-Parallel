#include <stdio.h>
#include <stdlib.h>
#include<time.h>
#include <mpi.h>

#define ARRAY_SIZE 10

int binarySearch(int arr[], int target, int left, int right)
{
	/*
	 * Binary Search Algorithm
	 */
    left = 0;
    right = strlen(arr)-1;
	int mid_point = (left + right) / 2;

	if(arr[mid_point] == target)
		return mid_point;
	else if(left>right)
		return -1;
    while(left<=right){
        if(arr[mid_point]<target){
            left=mid_point+1;
        }else if(arr[mid_point]==target){
            printf("Target %d found at index %d",arr[mid_point],mid_point);
            break;
        }else{
            right=mid_point-1;
            mid_point=(right+left)/2;
        }
    }
	// else if(target > arr[mid_point])
	// 	return binarySearch(arr, target, mid_point + 1, right);
	// else
	// 	return binarySearch(arr, target, left, mid_point - 1);

	// return -1;
}

void insertionSort(int arr[], int n)
{
	/*
	 * Insertion Sort Algorithm
	 */

	int i, j, key;

	for(i = 1; i < n; ++i)
	{
		key = arr[i];
		j = i - 1;
		while(j >= 0 && key < arr[j])
		{
			arr[j + 1] = arr[j];
			j = j  - 1;
		}
		arr[j + 1] = key;
	}

}

int main(int argc, char** argv)
{
	static int arr[ARRAY_SIZE];
	time_t t;
	int i;
	size_t n = sizeof(arr)/sizeof(arr[0]);

	// a data struct that provides more information on the received  message
	MPI_Status status;

	// Initialize the MPI environment
	MPI_Init(NULL, NULL);

	// Get the rank of the process
	int pid;
	MPI_Comm_rank(MPI_COMM_WORLD, &pid);

	// Get the number of processes
	int number_of_processes;
	MPI_Comm_size(MPI_COMM_WORLD, &number_of_processes);

	if (pid == 0) {

		// master process

		srand((unsigned) time(&t));

		for( i = 0 ; i < n ; ++i )
			arr[i] = rand() % 50;


		int index, i;
		int elements_per_process;

		srand((unsigned) time(&t));
		int key = rand() % 50;


		elements_per_process = ARRAY_SIZE / number_of_processes;

		// check if more than 1 processes are running
		if (number_of_processes > 1) {
			// distributes the portion of the array among all processes
			for (i = 1; i < number_of_processes - 1; i++) {
				index = i * elements_per_process;

				MPI_Send(&key,
						1, MPI_INT, i, 0,
						MPI_COMM_WORLD);
				MPI_Send(&elements_per_process,
						1, MPI_INT, i, 0,
						MPI_COMM_WORLD);
				MPI_Send(&arr[index],
						elements_per_process,
						MPI_INT, i, 0,
						MPI_COMM_WORLD);
			}

			// last process adds remaining elements
			index = i * elements_per_process;
			int elements_left = ARRAY_SIZE - index;

			MPI_Send(&key,
					1, MPI_INT,
					i, 0,
					MPI_COMM_WORLD);
			MPI_Send(&elements_left,
					1, MPI_INT,
					i, 0,
					MPI_COMM_WORLD);
			MPI_Send(&arr[index],
					elements_left,
					MPI_INT, i, 0,
					MPI_COMM_WORLD);
		}

		// master process sorts its portion of the array
		insertionSort(arr, elements_per_process);


		index = binarySearch(arr, key, 0, elements_per_process);

		// master process collects the searching result
		if (index == - 1)
			for (i = 1; i < number_of_processes; i++) {
				MPI_Recv(&index, 1, MPI_INT,
						MPI_ANY_SOURCE, 0,
						MPI_COMM_WORLD,
						&status);
				if (index == -1)
					printf("the key %d is not in the array\n", key);
				else
					printf("the key %d is found in the array\n", key);
			}
		else
			printf("the key %d is found in the array\n", key);

	} else
	{
		// worker processes

		int key = 0;

		// receive the key
		MPI_Recv(&key,
				1, MPI_INT, 0, 0,
				MPI_COMM_WORLD,
				&status);

		// receive the size of its portion of the array
		int num_of_elements_recieved = 0;
		MPI_Recv(&num_of_elements_recieved,
				1, MPI_INT, 0, 0,
				MPI_COMM_WORLD,
				&status);

		int buffer[num_of_elements_recieved];
		size_t n = sizeof(buffer)/sizeof(buffer[0]);

		// receive its portion of the array and store it in buffer
		MPI_Recv(&buffer, num_of_elements_recieved,
				MPI_INT, 0, 0,
				MPI_COMM_WORLD,
				&status);

		// sort its portion of the array
		insertionSort(buffer, n);

		// search for the key in its portion of the array
		int index = binarySearch(buffer, key, 0, n);

		// send the searching result to the master process
		MPI_Send(&index, 1, MPI_INT,
				0, 0, MPI_COMM_WORLD);
	}

	// Finalize the MPI environment
	MPI_Finalize();

	return 0;
}
