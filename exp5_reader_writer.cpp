#include <chrono>
#include <iostream>
#include <mutex>
#include <semaphore.h>
#include <thread>
#include <vector>
using namespace std;
// Shared resource and synchronization tools
int read_count = 0; // Number of readers currently accessing the shared resource
mutex mtx;          // Mutex to protect access to read_count
sem_t w;            // Semaphore for writers
const int max_reads = 3;  // Max read operations per reader
const int max_writes = 2; // Max write operations per writer
void writer(int writer_id) {
  for (int i = 0; i < max_writes; ++i) {
    sem_wait(&w); // Writer requests to enter critical section
    // Critical section (writing)
    cout << "Writer " << writer_id << " is writing to the shared resource."
         << endl;
    this_thread::sleep_for(
        chrono::milliseconds(1000)); // Simulate write operation
    cout << "Writer " << writer_id << " has finished writing." << endl;
    sem_post(&w); // Writer leaves the critical section
    this_thread::sleep_for(chrono::milliseconds(
        1000)); // Simulate time before the next write attempt
  }
}
void reader(int reader_id) {
  for (int i = 0; i < max_reads; ++i) {
    // Request to enter the critical section
    mtx.lock();
    read_count++;
    if (read_count == 1) {
      sem_wait(&w); // First reader locks the writer
    }
    mtx.unlock();
    // Critical section (reading)
    cout << "Reader " << reader_id << " is reading the shared resource."
         << endl;
    this_thread::sleep_for(
        chrono::milliseconds(500)); // Simulate read operation
    cout << "Reader " << reader_id << " has finished reading." << endl;
    // Exit the critical section
    mtx.lock();
    read_count--;
    if (read_count == 0) {
      sem_post(&w); // Last reader unlocks the writer
    }
    mtx.unlock();
    this_thread::sleep_for(chrono::milliseconds(
        1000)); // Simulate time before the next read attempt
  }
}
int main() {
  // Initialize the semaphore for writers (initial value 1)
  sem_init(&w, 0, 1);
  // Create a few readers and writers (threads)
  vector<thread> readers, writers;
  // Launching 3 readers and 2 writers
  for (int i = 0; i < 3; ++i) {
    readers.push_back(thread(reader, i + 1));
  }
  for (int i = 0; i < 2; ++i) {
    writers.push_back(thread(writer, i + 1));
  }
  // Join the threads (they will stop after the set number of reads/writes)
  for (auto &t : readers) {
    t.join();
  }
  for (auto &t : writers) {
    t.join();
  }
  // Destroy the semaphor
  sem_destroy(&w);
  cout << "\nAll readers and writers have finished their operations, program "
          "terminating"
       << endl;
  return 0;
}
