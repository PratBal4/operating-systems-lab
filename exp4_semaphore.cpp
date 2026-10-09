//USING SEMAPHORE
#include <iostream>
#include <thread>
#include <vector>
#include <random>
#include <chrono>
#include <mutex>
#include <condition_variable>
using namespace std;
const int Delayx = 60;
const int max_visits_per_person = 2; // Number of times each person visits the restroom
int restroom = 0;
int Menwaiting = 0;
int Womenwaiting = 0;
class Semaphore {
private:
 mutex mtx;
 condition_variable cv;
 int count;
public:
 Semaphore(int count_ = 0) : count(count_) {}
 void acquire() {
 unique_lock<mutex> lock(mtx);
 cv.wait(lock, [this]() { return count > 0; });
 --count;
 }
 void release() {
 unique_lock<mutex> lock(mtx);
 ++count;
 cv.notify_one();
 }
};
// Custom semaphores
Semaphore max_capacity(4); // Restroom capacity of 4
Semaphore woman(1);
Semaphore man(1);
mutex mtx; // Mutex to protect shared resources
mutex restroomcount_mtx;
std::random_device rd;
std::mt19937 gen(rd()); // Random number generator
std::uniform_int_distribution<> dis(1, Delayx);
void Delay(void)
{
 int DelayTime = dis(gen);
 this_thread::sleep_for(chrono::milliseconds(DelayTime * 10)); // Simulate some delay
}
void Woman(int id) 
{
 for (int visit = 0; visit < max_visits_per_person; ++visit)
 {
 {
 lock_guard<mutex> lock(mtx);
 Womenwaiting++;
 }
 woman.acquire();
 max_capacity.acquire();
 {
 lock_guard<mutex> lock(restroomcount_mtx);
 restroom++;
 cout << "Woman " << id << " has entered the Restroom" << endl;
 cout << "People in the Restroom: " << restroom << endl << endl;
 }
 {
 lock_guard<mutex> lock(mtx);
 Womenwaiting--;
 }
 Delay();
 {
 lock_guard<mutex> lock(restroomcount_mtx);
 restroom--;
 cout << "Woman " << id << " has exited the Restroom" << endl;
 cout << "People in the Restroom: " << restroom << endl << endl;
 }
 max_capacity.release();
 if (Menwaiting > Womenwaiting)
 {
 man.release();
 }
 else
 {
 woman.release();
 }
 }
}
void Man(int id)
{
 for (int visit = 0; visit < max_visits_per_person; ++visit)
 {
 {
 lock_guard<mutex> lock(mtx);
 Menwaiting++;
 }
 man.acquire();
 max_capacity.acquire();
 {
 lock_guard<mutex> lock(restroomcount_mtx);
 restroom++;
 cout << "Man " << id << " has entered the Restroom" << endl;
 cout << "People in the Restroom: " << restroom << endl << endl;
 }
 {
 lock_guard<mutex> lock(mtx); 
 Menwaiting--;
 }
 Delay();
 {
 lock_guard<mutex> lock(restroomcount_mtx);
 restroom--;
 cout << "Man " << id << " has exited the Restroom" << endl;
 cout << "People in the Restroom: " << restroom << endl << endl;
 }
 max_capacity.release();
 if (Womenwaiting > Menwaiting)
 {
 woman.release();
 }
 else
 {
 man.release();
 }
 }
}
int main()
{
 vector<thread> threads;
 // Launch 2 women and 2 men, with unique IDs
 for (int i = 0; i < 2; ++i)
 {
 threads.push_back(thread(Woman, i + 1)); // Women threads with unique IDs
 threads.push_back(thread(Man, i + 1)); // Men threads with unique IDs
 }
 // Join the threads (they will stop after the set number of visits)
 for (auto& t : threads)
 {
 t.join();
 }
 cout << "All threads have finished, program terminating." << endl;
 return 0;
} 
