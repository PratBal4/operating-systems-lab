// Experiment 14 (Part A): Simulation of Indexed File Allocation Strategy
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    int f[50], index[50], i, n, j, c, k, ind, count = 0;

    // Initialize all disk blocks as unallocated (0)
    for (i = 0; i < 50; i++)
        f[i] = 0;

x:
    printf("Enter the index block: ");
    if (scanf("%d", &ind) != 1)
        return 0;

    if (ind < 0 || ind >= 50) {
        printf("Invalid index block! Must be between 0 and 49.\n");
        goto x;
    }

    if (f[ind] != 1) {
        printf("Enter no of blocks needed and no of files for the index %d on the disk : \n", ind);
        if (scanf("%d", &n) != 1)
            return 0;
    } else {
        printf("%d index is already allocated \n", ind);
        goto x;
    }

y:
    count = 0;
    for (i = 0; i < n; i++) {
        if (scanf("%d", &index[i]) != 1)
            return 0;
        if (index[i] >= 0 && index[i] < 50 && f[index[i]] == 0)
            count++;
    }

    if (count == n) {
        for (j = 0; j < n; j++)
            f[index[j]] = 1;
        f[ind] = 1;
        printf("Allocated\n");
        printf("File Indexed\n");
        for (k = 0; k < n; k++)
            printf("%d-------->%d : %d\n", ind, index[k], f[index[k]]);
    } else {
        printf("File in the index is already allocated \n");
        printf("Enter another file indexed\n");
        goto y;
    }

    printf("Do you want to enter more file(Yes - 1/No - 0): ");
    if (scanf("%d", &c) != 1)
        return 0;

    if (c == 1)
        goto x;
    else
        return 0;
}
