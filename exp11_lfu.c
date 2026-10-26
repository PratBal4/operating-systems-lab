// Experiment 11 (Part B): Least Frequently Used (LFU) Page Replacement Algorithm
#include <stdio.h>
#include <limits.h>

int checkHit(int incomingPage, int queue[], int occupied) {
    for (int i = 0; i < occupied; i++) {
        if (incomingPage == queue[i])
            return i;
    }
    return -1;
}

void printFrame(int queue[], int occupied) {
    for (int i = 0; i < occupied; i++)
        printf("%d\t\t\t", queue[i]);
}

int main() {
    int incomingStream[] = {7, 0, 1, 2, 0, 3, 0, 4, 2, 3, 0, 3, 2, 1};
    int n = sizeof(incomingStream) / sizeof(incomingStream[0]);
    int frames = 3;
    int queue[3];
    int freq[3];
    int arrival[3];
    int occupied = 0;
    int pagefault = 0;

    for (int i = 0; i < frames; i++) {
        queue[i] = -1;
        freq[i] = 0;
        arrival[i] = 0;
    }

    printf("Page\t Frame1 \t Frame2 \t Frame3\n");

    for (int i = 0; i < n; i++) {
        printf("%d: \t\t", incomingStream[i]);
        int hitIndex = checkHit(incomingStream[i], queue, occupied);

        // Page HIT: increment reference frequency
        if (hitIndex != -1) {
            freq[hitIndex]++;
            printFrame(queue, occupied);
        }
        // Filling empty frame
        else if (occupied < frames) {
            queue[occupied] = incomingStream[i];
            freq[occupied] = 1;
            arrival[occupied] = i;
            occupied++;
            pagefault++;
            printFrame(queue, occupied);
        }
        // Page FAULT: Replace page with Least Frequently Used (LFU)
        else {
            int minFreq = INT_MAX;
            int replaceIndex = -1;
            int oldestArrival = INT_MAX;

            for (int j = 0; j < frames; j++) {
                if (freq[j] < minFreq) {
                    minFreq = freq[j];
                    replaceIndex = j;
                    oldestArrival = arrival[j];
                } else if (freq[j] == minFreq) {
                    // Tie-breaker: FIFO (oldest arrival)
                    if (arrival[j] < oldestArrival) {
                        replaceIndex = j;
                        oldestArrival = arrival[j];
                    }
                }
            }

            queue[replaceIndex] = incomingStream[i];
            freq[replaceIndex] = 1;
            arrival[replaceIndex] = i;
            pagefault++;
            printFrame(queue, occupied);
        }
        printf("\n");
    }

    printf("Page Fault: %d\n", pagefault);
    return 0;
}
