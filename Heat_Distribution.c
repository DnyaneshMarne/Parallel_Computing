/*
 ============================================================================
 Name        : HW2_Parallel.c
 Author      : Dnyanesh Marne
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#include <mpi.h>
#include <stdio.h>

#define meshSize 1000
#define twenty 20.00
#define threeHundred 300.00
#define REP 10000
#define WHITE    "15 15 15 "
#define RED      "15 00 00 "
#define ORANGE   "15 05 00 "
#define YELLOW   "15 10 00 "
#define LTGREEN  "00 13 00 "
#define GREEN    "05 10 00 "
#define LTBLUE   "00 05 10 "
#define BLUE     "00 00 10 "
#define DARKTEAL "00 05 05 "
#define BROWN    "03 03 00 "
#define BLACK    "00 00 00 "
 //method to copy segments
void copyToOld(int size, float oldArr[(meshSize / size) + 2][meshSize], float newArr[(meshSize / size) + 2][meshSize], int rank) {
	if (rank == 0) {
		for (int i = 1; i < (meshSize / size) + 2; i++) {
			for (int j = 0; j < meshSize; j++) {
				oldArr[i][j] = newArr[i][j];
			}
		}
	}
	else {
		for (int i = 0; i < (meshSize / size) + 2; i++) {
			for (int j = 0; j < meshSize; j++) {
				oldArr[i][j] = newArr[i][j];
			}
		}
	}

}
//method to calculate the heat average for given point
void calculateNew(int size, float oldArr[(meshSize / size) + 2][meshSize], float newArr[(meshSize / size) + 2][meshSize], int x, int y) {
	oldArr[x][y] = (newArr[x - 1][y] + newArr[x + 1][y] + newArr[x][y + 1] + newArr[x][y - 1]) / 4.0;

}
//method for creating the image of heat distribution
void createGridMap(float oldArr[meshSize][meshSize]) {
	FILE* fp = NULL;


	int color = 0;

	char* colors[10] = { RED, ORANGE, YELLOW, LTGREEN, GREEN,
						  LTBLUE, BLUE, DARKTEAL, BROWN, BLACK };

	int linelen = meshSize;
	int numlines = meshSize;
	int i, j;

	fp = fopen("heatMapParallel.pnm", "w");

	fprintf(fp, "P3\n%d %d\n15\n", linelen, numlines);

	for (i = 0; i < meshSize; i++) {
		for (j = 0; j < meshSize; j++) {
			if (oldArr[i][j] > 250.00) {
				color = 0;
			}
			else if (oldArr[i][j] < 250.00 && oldArr[i][j] > 180.00) {
				color = 1;
			}
			else if (oldArr[i][j] < 180.00 && oldArr[i][j] > 120.00) {
				color = 2;
			}
			else if (oldArr[i][j] < 120.00 && oldArr[i][j] > 80.00) {
				color = 3;
			}
			else if (oldArr[i][j] < 80.00 && oldArr[i][j] > 60.00) {
				color = 4;
			}
			else if (oldArr[i][j] < 60.00 && oldArr[i][j] > 50.00) {
				color = 5;
			}
			else if (oldArr[i][j] < 50.00 && oldArr[i][j] > 40.00) {
				color = 6;
			}
			else if (oldArr[i][j] < 40.00 && oldArr[i][j] > 30.00) {
				color = 7;
			}
			else if (oldArr[i][j] < 30.00 && oldArr[i][j] > 20.00) {
				color = 8;
			}
			else if (oldArr[i][j] <= 20.00) {
				color = 9;
			}
			fprintf(fp, "%s ", colors[color]);
		}
		fprintf(fp, "\n");
	}
	fclose(fp);

}
//Driver code
int main(int argc, char* argv[]) {
	int rank, size;
	MPI_Status status;

	int i, j, k;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	float combineSegments[meshSize + size * 2][meshSize];
	float segment1[(meshSize / size) + 2][meshSize];
	float segment2[(meshSize / size) + 2][meshSize];
	float mesh1[meshSize][meshSize];

	for (i = 0; i < meshSize; i++) {
		for (j = 0; j < meshSize; j++) {
			mesh1[i][j] = twenty;
			if (i == 0 && (j < ((meshSize / 2) + meshSize * 0.2) && j >= ((meshSize / 2) - meshSize * 0.2))) {
				mesh1[i][j] = threeHundred;
			}

		}
	}
	for (i = 0; i < (meshSize / size) + 2; i++) {
		for (j = 0; j < meshSize; j++) {
			if (i >= (meshSize / size)) {
				segment1[i][j] = 20.00;
				segment2[i][j] = 20.00;
			}
			else {
				segment1[i][j] = mesh1[(meshSize / size) * rank + i][j];
				segment2[i][j] = mesh1[(meshSize / size) * rank + i][j];
			}
		}
	}



	int sendbuf = ((meshSize / size) + 2) * (meshSize);
	for (k = 0; k < REP; k++) {
		// Communication logic
		if (rank == 0) {
			MPI_Send(segment1[meshSize / size], meshSize, MPI_FLOAT, rank + 1, 0, MPI_COMM_WORLD);
			MPI_Recv(segment1[meshSize / size + 1], meshSize, MPI_FLOAT, rank + 1, 0, MPI_COMM_WORLD, &status);
		}
		else if (rank != size - 1) {
			MPI_Send(segment1[1], meshSize, MPI_FLOAT, rank - 1, 0, MPI_COMM_WORLD);
			MPI_Recv(segment1[0], meshSize, MPI_FLOAT, rank - 1, 0, MPI_COMM_WORLD, &status);

			MPI_Recv(segment1[meshSize / size + 1], meshSize, MPI_FLOAT, rank + 1, 0, MPI_COMM_WORLD, &status);
			MPI_Send(segment2[meshSize / size], meshSize + 2, MPI_FLOAT, rank + 1, 0, MPI_COMM_WORLD);
		}
		else if (rank == size - 1) {
			MPI_Send(segment1[1], meshSize, MPI_FLOAT, rank - 1, 0, MPI_COMM_WORLD);
			MPI_Recv(segment1[0], meshSize, MPI_FLOAT, rank - 1, 0, MPI_COMM_WORLD, &status);
		}

		MPI_Barrier(MPI_COMM_WORLD);
		MPI_Gather(segment2, sendbuf, MPI_FLOAT, combineSegments, sendbuf, MPI_FLOAT, 0, MPI_COMM_WORLD);

		copyToOld(size, segment2, segment1, rank);
		for (i = 1; i < meshSize / size + 2; i++) {
			for (j = 2; j < meshSize - 2; j++) {
				//call to calculate method
				calculateNew(size, segment1, segment2, i, j);
			}
		}

	}

	if (rank == 0) {
		printf("---------------------------------------------------\n");
		for (i = 0; i < meshSize + size * 2; i++) {
			for (j = 0; j < meshSize; j++) {
				printf("%f ", combineSegments[i][j]);
			}
			printf("\n");
		}
	}
	//call to image creation method  
	createGridMap(combineSegments);
	MPI_Finalize();
	return 0;
}



