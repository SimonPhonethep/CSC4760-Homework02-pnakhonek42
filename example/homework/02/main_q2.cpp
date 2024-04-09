#include <mpi.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace std;

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int processRank, totalProcesses;
    MPI_Comm_rank(MPI_COMM_WORLD, &processRank);
    MPI_Comm_size(MPI_COMM_WORLD, &totalProcesses);

    int gridRows;
    int gridCols;
    int iterations;

    if (processRank == 0) {
        if (argc != 4) {
            cout << "Usage: mpirun -n <number of processes> ./init <gridRows> <gridCols> <iterations>" << endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        gridRows = atoi(argv[1]);
        gridCols = atoi(argv[2]);
        iterations = atoi(argv[3]);
        if (gridRows < 1 || gridCols < 1 || iterations < 1) {
            cout << "gridRows, gridCols, iterations must be greater than 0" << endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
    }

    MPI_Bcast(&gridRows, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&gridCols, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&iterations, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int localRows = gridRows / totalProcesses;
    if (processRank == (totalProcesses-1)){
        localRows += gridRows % totalProcesses; 
    }

    int localRowsWithGhost = localRows + 2;
    int colsWithGhost = gridCols + 2;

    vector<vector<int>> currentGrid(localRowsWithGhost, vector<int>(colsWithGhost, 0));
    vector<vector<int>> nextGrid(localRowsWithGhost, vector<int>(colsWithGhost, 0));

    srand(time(0));
    for (int iRow = 1; iRow <= localRows; iRow++){
        for (int iCol = 1; iCol <= gridCols; iCol++){
            currentGrid[iRow][iCol] = rand() % 2;
        }
    }

    int upperProcess = (processRank == 0) ? totalProcesses - 1 : processRank - 1;
    int lowerProcess = (processRank == totalProcesses - 1) ? 0 : processRank + 1;
    
    const int ALIVE = 1;
    const int DEAD = 0;
    
    for (int iTime = 0; iTime < iterations; iTime++){
        MPI_Send(&currentGrid[1][1], gridCols, MPI_INT, upperProcess, 0, MPI_COMM_WORLD);
        MPI_Send(&currentGrid[localRows][1], gridCols, MPI_INT, lowerProcess, 0, MPI_COMM_WORLD);
        MPI_Recv(&currentGrid[localRows + 1][1], gridCols, MPI_INT, lowerProcess, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&currentGrid[0][1], gridCols, MPI_INT, upperProcess, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Send(&currentGrid[1][1], 1, MPI_INT, upperProcess, 0, MPI_COMM_WORLD);
        MPI_Send(&currentGrid[localRows][1], 1, MPI_INT, lowerProcess, 0, MPI_COMM_WORLD);
        MPI_Recv(&currentGrid[localRows + 1][0], 1, MPI_INT, lowerProcess, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&currentGrid[0][0], 1, MPI_INT, upperProcess, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        
        for (int iRow = 1; iRow <= localRows; iRow++){
            for (int iCol = 1; iCol <= gridCols; iCol++){
                int aliveNeighbors = 0;
                for (int jRow = iRow - 1; jRow <= iRow + 1; jRow++){
                    for (int jCol = iCol - 1; jCol <= iCol + 1; jCol++){
                        int row = (jRow + localRowsWithGhost) % localRowsWithGhost; 
                        int col = (jCol + colsWithGhost) % colsWithGhost; 
                        if ((jRow != iRow || jCol != iCol) && currentGrid[row][col] == ALIVE){
                            aliveNeighbors++;
                        }
                    }
                }

                if (currentGrid[iRow][iCol] == ALIVE){
                    nextGrid[iRow][iCol] = (aliveNeighbors == 2 || aliveNeighbors == 3) ? ALIVE : DEAD;
                } else {
                    nextGrid[iRow][iCol] = (aliveNeighbors == 3) ? ALIVE : DEAD;
                }
            }
        }

        for (int iRow = 1; iRow <= localRows; iRow++){
            for (int iCol = 1; iCol <= gridCols; iCol++){
                currentGrid[iRow][iCol] = nextGrid[iRow][iCol];
            }
        }

        MPI_Barrier(MPI_COMM_WORLD);

        for (int iRow = 1; iRow <= localRows; iRow++){
            for (int iCol = 1; iCol <= gridCols; iCol++){
                cout << (currentGrid[iRow][iCol] == ALIVE ? "X" : "-") << " ";
            }
            cout << endl;
        }

        MPI_Barrier(MPI_COMM_WORLD);

        cout << "Process: " << processRank << ", Iteration: " << iTime << endl;
    }

    MPI_Finalize();    
    return 0;
}