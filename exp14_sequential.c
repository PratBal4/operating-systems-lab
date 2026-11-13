// Experiment 14 (Part B): Simulation of Sequential File Allocation Strategy
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    int f[50], i, st, len, j, c, k, count = 0;

    // Initialize all disk blocks as free (0)
    for (i = 0; i < 50; i++)
        f[i] = 0;

    printf("Files Allocated are : \n");

x:
    count = 0;
    printf("Enter starting block and length of files: ");
    if (scanf("%d%d", &st, &len) != 2)
        return 0;

    if (st >= 0 && (st + len) <= 50) {
        for (k = st; k < (st + len); k++) {
            if (f[k] == 0)
                count++;
        }
    }

    if (len > 0 && len == count) {
        for (j = st; j < (st + len); j++) {
            if (f[j] == 0) {
                f[j] = 1;
                printf("%d\t%d\n", j, f[j]);
            }
        }
        printf("The file is allocated to disk\n");
    } else {
        printf("The file is not allocated \n");
    }

    printf("Do you want to enter more file(Yes - 1/No - 0): ");
    if (scanf("%d", &c) != 1)
        return 0;

    if (c == 1)
        goto x;
    else
        return 0;
}

