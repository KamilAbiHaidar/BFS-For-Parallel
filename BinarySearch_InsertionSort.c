#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

#define ARRAY_SIZE 1000000

// Binary Search
int binarySearch(int arr[], int target, int left, int right)
{

	int mid_point = (left + right) / 2;
	if (arr[mid_point] == target)
		return mid_point;

	else if (left > right)
		return -1;

	else if (target > arr[mid_point])
		return binarySearch(arr, target, mid_point + 1, right);
	else
		return binarySearch(arr, target, left, mid_point - 1);

	return -1;
}

// Insertion sort
void insertionSort(int arr[], int n)
{
	int i;
	int j;
	int target;

	for (i = 1; i < n; ++i)
	{
		target = arr[i];
		j = i - 1;
		while (j >= 0 && target < arr[j])
		{
			arr[j + 1] = arr[j];
			j = j - 1;
		}
		arr[j + 1] = target;
	}
}

int main(int argc, char **argv)
{
	static int arr[ARRAY_SIZE];
	time_t t;
	int i;
	size_t n = sizeof(arr) / sizeof(arr[0]);
	int target;

	MPI_Status status;

	// MPI Environment
	// Initialize the MPI environment
	MPI_Init(&argc, &argv);
	// Get the rank of the process
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	// Get the number of processes
	int size;
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	// Get runtime
	MPI_Barrier(MPI_COMM_WORLD);
	double start = MPI_Wtime();

	// Master process
	if (rank == 0)
	{
		srand((unsigned)time(&t));

		for (i = 0; i < n; ++i)
			arr[i] = rand() % 50;

		int index;
		int i;
		int num_per_process;

		srand((unsigned)time(&t));

		target = rand() % 50;

		num_per_process = ARRAY_SIZE / size;

		if (size > 1)
		{

			for (i = 1; i < size - 1; i++)
			{
				index = i * num_per_process;

				MPI_Send(&target, 1, MPI_INT, i, 0, MPI_COMM_WORLD);

				MPI_Send(&num_per_process, 1, MPI_INT, i, 1, MPI_COMM_WORLD);

				MPI_Send(&arr[index], num_per_process, MPI_INT, i, 2, MPI_COMM_WORLD);
				// printf("Send to process %d",i);
			}

			index = i * num_per_process;
			int elements_left = ARRAY_SIZE - index;

			MPI_Send(&target, 1, MPI_INT, i, 0, MPI_COMM_WORLD);

			MPI_Send(&elements_left, 1, MPI_INT, i, 1, MPI_COMM_WORLD);

			MPI_Send(&arr[index], elements_left, MPI_INT, i, 2, MPI_COMM_WORLD);
			// printf("Send to process %d",i);
		}

		insertionSort(arr, num_per_process);

		index = binarySearch(arr, target, 0, num_per_process);

		if (index == -1)
		{
			for (i = 1; i < size; i++)
			{
				MPI_Recv(&index, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
				if (index == -1)
				{
					// printf("Process %d", status.MPI_SOURCE);
					printf("The target %d is not found in the array\n", target);
				}
				else
				{
					// printf("Process %d", status.MPI_SOURCE);
					printf("The target %d is found in the array\n", target);
				}
			}
		}
		else
		{
			// printf("Process ABDALLAH");
			printf("The target %d is found in the array\n", target);
		}
	}
	else
	{

		target = 0;

		MPI_Recv(&target, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

		int num_of_elements_received = 0;
		MPI_Recv(&num_of_elements_received, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);

		int buffer[num_of_elements_received];
		size_t n = sizeof(buffer) / sizeof(buffer[0]);

		MPI_Recv(&buffer, num_of_elements_received, MPI_INT, 0, 2, MPI_COMM_WORLD, &status);

		insertionSort(buffer, n);

		int index = binarySearch(buffer, target, 0, n);
		printf("%d ", index);

		MPI_Send(&index, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
	}

	MPI_Barrier(MPI_COMM_WORLD);
	double right = MPI_Wtime();

	if (rank == 0)
	{
		printf("Total time parallel: %fs\n", right - start);

		clock_t start_t, end_t, total_t;
		int i;

		start_t = clock();
		printf("Starting of the program, start_t = %ld\n", start_t);
		insertionSort(arr, ARRAY_SIZE);
		if (binarySearch(arr, target, 0, ARRAY_SIZE) == -1)
		{
			printf("The target %d is not found in the array serially\n", target);
		}
		else
		{
			printf("The target %d is found in the array serially\n", target);
		}

		end_t = clock();
		printf("End of the big loop, end_t = %ld\n", end_t);

		printf("Serially:%Lf!", (long double)(end_t - start_t)/CLOCKS_PER_SEC);
	}

	// Finalize the MPI environment 
	MPI_Finalize();

	return 0;
}