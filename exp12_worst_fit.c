// Experiment 12 (Part B): Worst Fit Memory Management Policy
#include <stdio.h>

void implementWorstFit(int blockSize[], int blocks, int processSize[], int processes) {
    // Stores block id of allocated block to each process
    int allocation[processes];
    int occupied[blocks];

    // Initially assigning -1 to all allocation indexes (nothing allocated currently)
    for (int i = 0; i < processes; i++) {
        allocation[i] = -1;
    }

    for (int i = 0; i < blocks; i++) {
        occupied[i] = 0;
    }

    // Pick each process and find suitable blocks according to Worst Fit
    for (int i = 0; i < processes; i++) {
        int indexPlaced = -1;
        for (int j = 0; j < blocks; j++) {
            // If not occupied and block size is large enough
            if (blockSize[j] >= processSize[i] && !occupied[j]) {
                // Place it at the first block fit to accommodate process
                if (indexPlaced == -1)
                    indexPlaced = j;
                // If any future block is larger, change indexPlaced (Worst Fit)
                else if (blockSize[indexPlaced] < blockSize[j])
                    indexPlaced = j;
            }
        }

        // If a block was successfully found for the process
        if (indexPlaced != -1) {
            // Allocate this block to process
            allocation[i] = indexPlaced;

            // Make the status of the block as occupied
            occupied[indexPlaced] = 1;

            // Reduce available memory for the block
            blockSize[indexPlaced] -= processSize[i];
        }
    }

    printf("\nProcess No.\tProcess Size\tBlock no.\n");
    for (int i = 0; i < processes; i++) {
        printf("%d \t\t\t %d \t\t\t", i + 1, processSize[i]);
        if (allocation[i] != -1)
            printf("%d\n", allocation[i] + 1);
        else
            printf("Not Allocated\n");
    }
}

// Driver code
int main() {
    int blockSize[] = {100, 50, 30, 120, 35};
    int processSize[] = {40, 10, 30, 60};
    int blocks = sizeof(blockSize) / sizeof(blockSize[0]);
    int processes = sizeof(processSize) / sizeof(processSize[0]);

    implementWorstFit(blockSize, blocks, processSize, processes);

    return 0;
}
