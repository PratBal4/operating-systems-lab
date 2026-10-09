// USING MONITOR
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>
class NinjaMonitor {
private:
  int SharedResource;
  std::mutex mtx_;
  std::condition_variable CondVar;

public:
  // Constructor to initialize the shared resource and synchronization variables
  NinjaMonitor() : SharedResource(0) {}
  // Method to access the shared resource
  int AccessSharedResource() {
    std::unique_lock<std::mutex> lock(mtx_);
    while (SharedResource == 0) {
      CondVar.wait(lock);
    }
    int value = SharedResource;
    SharedResource = 0;
    return value;
  }
  // Method to update the shared resource
  void UpdateSharedResource(int new_value) {
    std::unique_lock<std::mutex> lock(mtx_);
    SharedResource = new_value;
    CondVar.notify_one();
  }
};
int it = 0;
// Function that uses the monitor to access the shared resource
void NinjaThread(NinjaMonitor &monitor) {
  int value = monitor.AccessSharedResource();
  std::cout << "Ninja thread read shared resource " << it << ":" << value
            << std::endl;
  it++;
}
int main() {
  NinjaMonitor monitor;
  std::thread t(NinjaThread, std::ref(monitor));
  std::thread t1(NinjaThread, std::ref(monitor));
  // Update the shared resource after some time

  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  monitor.UpdateSharedResource(67);
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  monitor.UpdateSharedResource(89);
  t.join();
  t1.join();
  return 0;
}
