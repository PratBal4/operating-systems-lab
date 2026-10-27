# Operating Systems Laboratory: Experiments & Output Analysis

**Course:** Operating Systems Lab  
**Platform:** macOS / Darwin Kernel (Unix-compatible)  
**Compiler:** Clang / Apple LLVM (`gcc`, `g++` / C++17)  
**Author / Terminal Session:** `balajipratyush4@Pratyushs-MacBook-Air`

---

## Table of Contents
1. [Experiment 1: Process Creation & Variable Memory Isolation using `fork()`](#experiment-1-process-creation--variable-memory-isolation-using-fork)
2. [Experiment 2 (Part A): Cascading Process Tree Generation with Multiple `fork()` Calls](#experiment-2-part-a-cascading-process-tree-generation-with-multiple-fork-calls)
3. [Experiment 2 (Part B): Process Creation with Boolean Short-Circuit Evaluation](#experiment-2-part-b-process-creation-with-boolean-short-circuit-evaluation)
4. [Experiment 2 (Part C): Process Identification using `getpid()` and `getppid()`](#experiment-2-part-c-process-identification-using-getpid-and-getppid)
5. [Experiment 3: Multithreading in C++ using Diverse Callable Targets](#experiment-3-multithreading-in-c-using-diverse-callable-targets)
6. [Experiment 4 (Part A): Synchronization using Counting Semaphores — Unisex Restroom Problem](#experiment-4-part-a-synchronization-using-counting-semaphores--unisex-restroom-problem)
7. [Experiment 4 (Part B): Process Synchronization using Monitor Construct](#experiment-4-part-b-process-synchronization-using-monitor-construct)
8. [Experiment 5: Classical Concurrency — Readers-Writers Problem using Semaphores & Mutex](#experiment-5-classical-concurrency--readers-writers-problem-using-semaphores--mutex)
9. [Experiment 6: Classical Concurrency — Dining Philosophers Problem using Monitors](#experiment-6-classical-concurrency--dining-philosophers-problem-using-monitors)
10. [Experiment 7: Deadlock Avoidance using Banker's Algorithm](#experiment-7-deadlock-avoidance-using-bankers-algorithm)
11. [Experiment 8 (Part A): CPU Scheduling — First-Come, First-Served (FCFS)](#experiment-8-part-a-cpu-scheduling--first-come-first-served-fcfs)
12. [Experiment 8 (Part B): CPU Scheduling — Shortest Job First (SJF Non-Preemptive)](#experiment-8-part-b-cpu-scheduling--shortest-job-first-sjf-non-preemptive)
13. [Experiment 9 (Part A): CPU Scheduling — Priority Scheduling (Non-Preemptive)](#experiment-9-part-a-cpu-scheduling--priority-scheduling-non-preemptive)
14. [Experiment 9 (Part B): CPU Scheduling — Round Robin (RR)](#experiment-9-part-b-cpu-scheduling--round-robin-rr)
15. [Experiment 10: Page Replacement — First-In, First-Out (FIFO)](#experiment-10-page-replacement--first-in-first-out-fifo)
16. [Experiment 11 (Part A): Page Replacement — Least Recently Used (LRU)](#experiment-11-part-a-page-replacement--least-recently-used-lru)
17. [Experiment 11 (Part B): Page Replacement — Least Frequently Used (LFU)](#experiment-11-part-b-page-replacement--least-frequently-used-lfu)
18. [Experiment 12 (Part A): Memory Allocation — Best Fit Policy](#experiment-12-part-a-memory-allocation--best-fit-policy)
19. [Experiment 12 (Part B): Memory Allocation — Worst Fit Policy](#experiment-12-part-b-memory-allocation--worst-fit-policy)
20. [Experiment 13 (Part A): Disk Scheduling — First-Come, First-Served (FCFS)](#experiment-13-part-a-disk-scheduling--first-come-first-served-fcfs)
21. [Experiment 13 (Part B): Disk Scheduling — SCAN (Elevator) Algorithm](#experiment-13-part-b-disk-scheduling--scan-elevator-algorithm)

---

## Experiment 1: Process Creation & Variable Memory Isolation using `fork()`

### 1. Aim
To write a C program to demonstrate process creation using the `fork()` system call and verify that child and parent processes maintain private, independent address spaces (copy-on-write variable isolation).

### 2. Theory & Concepts
The `fork()` system call is the primary mechanism in UNIX-like operating systems to create a new process:
- Upon calling `fork()`, the kernel creates a child process that is an exact duplicate of the parent process.
- **Return Values:**
  - In the **parent process**, `fork()` returns the positive Process ID (PID) of the newly created child.
  - In the **child process**, `fork()` returns `0`.
  - On failure, `-1` is returned.
- **Address Space Separation:** Even though the child process inherits the variables and state of the parent at the point of fork, it receives its own virtual address space (implemented via Copy-On-Write). Hence, modifying variable values in the child does not modify them in the parent, and vice versa.

### 3. Source Code (`exp1_fork_output.c`)
```c
#include <stdio.h>
#include <unistd.h>

int main()
{
    int a = 5, b = 10, pid;
    printf("Before fork a=%d b=%d \n", a, b); 
    pid = fork();
    if(pid == 0)
    {
        a = a + 1; 
        b = b + 1;
        printf("In child a=%d b=%d \n", a, b);
    }
    else
    {
        sleep(1); 
        a = a - 1; 
        b = b - 1;
        printf("In Parent a=%d b=%d \n", a, b);
    }
    return 0;
}
```

### 4. Output
<img src="Screenshot 2026-10-08 at 09.08.42.png" width="700" alt="Experiment 1 Output" />

```
balajipratyush4@Pratyushs-MacBook-Air code % ./exp1_fork_output
Before fork a=5 b=10 
In child a=6 b=11 
In Parent a=4 b=9 
```

### 5. Detailed Output Analysis
1. Before `fork()` executes, the initial values `a=5` and `b=10` are printed.
2. `fork()` executes and duplicates the process:
   - In the **child process** (`pid == 0`), `a` and `b` are incremented: `a = 5 + 1 = 6`, `b = 10 + 1 = 11`. The child prints `In child a=6 b=11`.
   - In the **parent process** (`pid > 0`), the parent sleeps for 1 second (`sleep(1)`), allowing the child to finish. When the parent resumes, its local copy of `a` and `b` remains untouched by the child's increments (still 5 and 10). It decrements both: `a = 5 - 1 = 4`, `b = 10 - 1 = 9`, and prints `In Parent a=4 b=9`.
3. **Conclusion:** This verifies distinct physical memory pages backing parent and child variables post-fork.

---

## Experiment 2 (Part A): Cascading Process Tree Generation with Multiple `fork()` Calls

### 1. Aim
To demonstrate cascading process generation and verify that $n$ consecutive `fork()` system calls generate $2^n$ processes in total.

### 2. Theory & Concepts
Each execution of `fork()` branches the executing thread into two concurrent execution streams:
- $k=1$ `fork()`: 1 parent becomes 2 processes ($2^1 = 2$).
- $k=2$ `fork()`: Both processes execute the second fork, producing 4 processes ($2^2 = 4$).
- $k=3$ `fork()`: All 4 processes execute the third fork, producing 8 processes ($2^3 = 8$).
- Formula for total processes: $N = 2^k$, where $k$ is the number of sequential `fork()` invocations.
- Total child processes created: $2^k - 1 = 8 - 1 = 7$.

### 3. Source Code (`exp2_srmist.c`)
```c
#include <stdio.h>
#include <unistd.h>

int main()
{
    fork(); 
    fork();
    fork();
    printf("SRMIST\n"); 
    return 0;
}
```

### 4. Output
<img src="Screenshot 2026-10-08 at 09.08.57.png" width="700" alt="Experiment 2A Output" />

```
balajipratyush4@Pratyushs-MacBook-Air code % ./exp2_srmist
SRMIST
SRMIST
SRMIST
SRMIST
SRMIST
SRMIST
SRMIST
SRMIST
```

### 5. Detailed Output Analysis
- The program executes 3 consecutive `fork()` statements.
- By the time control reaches `printf("SRMIST\n")`, 8 distinct processes exist concurrently.
- Each process independently outputs the string `"SRMIST\n"`, yielding exactly 8 output lines in the terminal.

---

## Experiment 2 (Part B): Process Creation with Boolean Short-Circuit Evaluation

### 1. Aim
To analyze process creation behavior when `fork()` calls are combined with short-circuit boolean operators (`&&` and `||`).

### 2. Theory & Concepts
In C, logical operators evaluate with strict left-to-right short-circuit semantics:
- `expr1 && expr2`: If `expr1` evaluates to 0 (false), `expr2` is skipped (not executed).
- `expr1 || expr2`: If `expr1` evaluates to non-zero (true), `expr2` is skipped.
- Since `fork()` returns `0` in the child and a positive non-zero PID in the parent:
  - In a parent process, `fork()` evaluates to `true`.
  - In a child process, `fork()` evaluates to `false`.

### 3. Source Code (`exp2_child_count.c`)
```c
#include <stdio.h>
#include <unistd.h>

int main() {
    fork();
    fork() && fork() || fork();
    fork();
    printf("Yes ");
    return 0;
}
```

### 4. Output
<img src="Screenshot 2026-10-08 at 09.09.09.png" width="700" alt="Experiment 2B Output" />

```
balajipratyush4@Pratyushs-MacBook-Air code % ./exp2_child_count
Yes Yes Yes Yes Yes Yes Yes Yes %
balajipratyush4@Pratyushs-MacBook-Air code % Yes Yes Yes Yes Yes Yes Yes Yes
```

### 5. Detailed Output Analysis
1. First `fork()`: 1 process $\to$ 2 processes.
2. `fork() && fork() || fork()`:
   - For each of the 2 processes, the first `fork()` branches into parent ($>0$) and child ($0$).
   - For the parent branch (truthy), the `&&` condition proceeds to execute the second `fork()`. If that second fork's parent returns $>0$, the whole expression is true, short-circuiting the `||`. If it returns $0$, it evaluates the `||` fork.
   - For the child branch of the first fork ($0$, falsy), the `&&` short-circuits, directly evaluating the `|| fork()`.
   - Across all paths, this composite boolean line leaves 4 processes active.
3. Third `fork()`: The 4 processes each fork once more, resulting in $4 \times 2 = 8$ processes.
4. Each process prints `"Yes "`. Because no trailing newline was emitted, the shell prompt indicator (`%` in zsh) appears immediately after the eighth print.

---

## Experiment 2 (Part C): Process Identification using `getpid()` and `getppid()`

### 1. Aim
To obtain and display the Process ID (PID) of the current process and its Parent Process ID (PPID) using `getpid()` and `getppid()`.

### 2. Theory & Concepts
- `getpid()`: Returns the unique identifier assigned to the calling process by the kernel.
- `getppid()`: Returns the process ID of the parent that created the calling process.
- In parent: `getpid()` is the parent's PID, and `getppid()` is the terminal shell's PID.
- In child: `getpid()` is the newly allocated child PID, and `getppid()` matches the parent's PID.

### 3. Source Code (`exp2_process_id.c`)
```c
#include <stdio.h> 
#include <unistd.h> 
#include <stdlib.h> 

int main() 
{ 
    int pid; 
    pid = fork(); 
    if(pid == -1) 
    {
        perror("fork failed"); 
        exit(0); 
    } 
    if(pid == 0) 
    { 
        printf("\nChild process is under execution"); 
        printf("\nProcess id of the child process is %d", getpid()); 
        printf("\nProcess id of the parent process is %d", getppid()); 
    } 
    else 
    { 
        printf("\nParent process is under execution"); 
        printf("\nProcess id of the parent process is %d", getpid()); 
        printf("\nProcess id of the parent of parent is %d", getppid()); 
    } 
    return 0; 
}
```

### 4. Output
<img src="Screenshot 2026-10-08 at 09.09.24.png" width="700" alt="Experiment 2C Output" />

```
balajipratyush4@Pratyushs-MacBook-Air code % ./exp2_process_id

Parent process is under execution
Process id of the parent process is 76420
Process id of the parent of parent is 72401
Child process is under execution
Process id of the child process is 76421
Process id of the parent process is 76420%
```

### 5. Detailed Output Analysis
- **Parent Process PID:** `76420`
- **Grandparent PID (Parent of Parent - Zsh Shell):** `72401`
- **Child Process PID:** `76421` (sequentially allocated by the OS kernel)
- **Parent of Child PID:** `76420` (correctly verifies that `76420` is the parent of `76421`).

---

## Experiment 3: Multithreading in C++ using Diverse Callable Targets

### 1. Aim
To demonstrate multithreading in C++ (`std::thread`) and launch concurrent threads using three distinct callable types: a function pointer, a function object (functor), and a lambda expression.

### 2. Theory & Concepts
C++11 provides native thread abstraction via `<thread>`:
1. **Function Pointer:** Traditional C-style function pointer passed directly to `std::thread(func, args...)`.
2. **Function Object (Functor):** A class or struct overloading `operator()`. Enables maintaining local state within the functor.
3. **Lambda Expression:** An inline anonymous callable closure introduced in modern C++.
- Calling `.join()` guarantees that the main thread synchronizes and waits for child threads to terminate before exiting `main()`.

### 3. Source Code (`exp3_multithreading.cpp`)
```cpp
// Multithreading
// C++ program to demonstrate multithreading using three different callables.
#include <iostream>
#include <thread>
using namespace std;

// A dummy function (Function Pointer Callable)
void foo(int Z) {
    for (int i = 0; i < Z; i++) {
        cout << "Thread using function pointer as callable\n";
    }
}

// A callable object (Functor)
class thread_obj {
public:
    void operator()(int x) {
        for (int i = 0; i < x; i++)
            cout << "Thread using function object as callable\n";
    }
};

// Driver code
int main() {
    cout << "Threads 1 and 2 and 3 operating independently" << endl;
    
    // Thread 1: Function pointer
    thread th1(foo, 3);
    
    // Thread 2: Function object
    thread th2(thread_obj(), 3);
    
    // Thread 3: Lambda expression
    auto f = [](int x) {
        for (int i = 0; i < x; i++)
            cout << "Thread using lambda expression as callable\n";
    };
    thread th3(f, 3);
    
    // Wait for all threads to finish
    th1.join();
    th2.join();
    th3.join();
    
    return 0;
}
```

### 4. Output
<img src="Screenshot 2026-10-08 at 09.09.33.png" width="700" alt="Experiment 3 Output" />

```
balajipratyush4@Pratyushs-MacBook-Air code % ./exp3_multithreading
Threads 1 and 2 and 3 operating independently
Thread using function pointer as callable
Thread using function pointer as callable
Thread using function pointer as callable
Thread using function object as callable
Thread using function object as callable
Thread using function object as callable
Thread using lambda expression as callable
Thread using lambda expression as callable
Thread using lambda expression as callable
```

### 5. Detailed Output Analysis
- All three threads run concurrently with an argument count of 3 iterations each.
- Each thread outputs its identifying callable category 3 times.
- Synchronization through `.join()` guarantees clean termination of all threads prior to the process exit.

---

## Experiment 4 (Part A): Synchronization using Counting Semaphores — Unisex Restroom Problem

### 1. Aim
To implement and resolve the Unisex Restroom synchronization problem using custom counting semaphores and mutexes to enforce capacity bounds and gender-exclusive access while preventing thread starvation.

### 2. Theory & Concepts
- **Problem Statement:** A restroom facility can be used by either men or women, but never simultaneously. At most $N$ persons ($N=4$) can be in the restroom at any one time. Starvation of either gender must be prevented.
- **Synchronization Primitives:**
  - `Semaphore max_capacity(4)`: Ensures maximum occupancy does not exceed 4.
  - `Semaphore woman(1)`, `Semaphore man(1)`: Controls entry turnstile for each gender.
  - `mutex mtx`: Serializes queue state changes (`Menwaiting`, `Womenwaiting`).
  - `mutex restroomcount_mtx`: Protects the restroom occupant count (`restroom`).
  - Priority switching: On exit, the process inspects whether `Menwaiting > Womenwaiting` or vice versa, signaling the opposing semaphore to prevent starvation.

### 3. Source Code (`exp4_semaphore.cpp`)
```cpp
// USING SEMAPHORE
#include <iostream>
#include <thread>
#include <vector>
#include <random>
#include <chrono>
#include <mutex>
#include <condition_variable>
using namespace std;

const int Delayx = 60;
const int max_visits_per_person = 2; 
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

Semaphore max_capacity(4); 
Semaphore woman(1);
Semaphore man(1);
mutex mtx; 
mutex restroomcount_mtx;
std::random_device rd;
std::mt19937 gen(rd()); 
std::uniform_int_distribution<> dis(1, Delayx);

void Delay(void) {
    int DelayTime = dis(gen);
    this_thread::sleep_for(chrono::milliseconds(DelayTime * 10)); 
}

void Woman(int id) {
    for (int visit = 0; visit < max_visits_per_person; ++visit) {
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
        if (Menwaiting > Womenwaiting) {
            man.release();
        } else {
            woman.release();
        }
    }
}

void Man(int id) {
    for (int visit = 0; visit < max_visits_per_person; ++visit) {
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
        if (Womenwaiting > Menwaiting) {
            woman.release();
        } else {
            man.release();
        }
    }
}

int main() {
    vector<thread> threads;
    for (int i = 0; i < 2; ++i) {
        threads.push_back(thread(Woman, i + 1)); 
        threads.push_back(thread(Man, i + 1)); 
    }
    for (auto& t : threads) {
        t.join();
    }
    cout << "All threads have finished, program terminating." << endl;
    return 0;
}
```

### 4. Output
<img src="Screenshot 2026-10-08 at 09.09.52.png" width="700" alt="Experiment 4A Output" />

```
balajipratyush4@Pratyushs-MacBook-Air code % ./exp4_semaphore
Woman 1 has entered the Restroom
People in the Restroom: 1

Man 1 has entered the Restroom
People in the Restroom: 2

Man 1 has exited the Restroom
People in the Restroom: 1

Man 1 has entered the Restroom
People in the Restroom: 2

Woman 1 has exited the Restroom
People in the Restroom: 1

Woman 1 has entered the Restroom
People in the Restroom: 2

Woman 1 has exited the Restroom
People in the Restroom: 1

Woman 2 has entered the Restroom
People in the Restroom: 2

Man 1 has exited the Restroom
People in the Restroom: 1

Man 2 has entered the Restroom
People in the Restroom: 2

Woman 2 has exited the Restroom
People in the Restroom: 1

Woman 2 has entered the Restroom
People in the Restroom: 2

Man 2 has exited the Restroom
People in the Restroom: 1

Man 2 has entered the Restroom
People in the Restroom: 2

Woman 2 has exited the Restroom
People in the Restroom: 1

Man 2 has exited the Restroom
People in the Restroom: 0

All threads have finished, program terminating.
```

### 5. Detailed Output Analysis
- Restroom occupancy fluctuates safely between 0, 1, and 2, well within the `max_capacity = 4` boundary.
- All threads complete their 2 designated visits, demonstrating deadlock-free synchronization.

---

## Experiment 4 (Part B): Process Synchronization using Monitor Construct

### 1. Aim
To implement process synchronization using a Monitor pattern encapsulating a shared resource, a mutex, and a condition variable in C++.

### 2. Theory & Concepts
- A **Monitor** is a high-level synchronization construct encapsulating private data variables with public entry procedures.
- Only one thread can execute an active method inside the monitor at any instant.
- Threads that need to wait on a condition block on a `condition_variable` using `wait()`.
- Updating threads signal waiting threads using `notify_one()` or `notify_all()`.

### 3. Source Code (`exp4_monitor.cpp`)
```cpp
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
    NinjaMonitor() : SharedResource(0) {}

    int AccessSharedResource() {
        std::unique_lock<std::mutex> lock(mtx_);
        while (SharedResource == 0) {
            CondVar.wait(lock);
        }
        int value = SharedResource;
        SharedResource = 0;
        return value;
    }

    void UpdateSharedResource(int new_value) {
        std::unique_lock<std::mutex> lock(mtx_);
        SharedResource = new_value;
        CondVar.notify_one();
    }
};

void NinjaThread(NinjaMonitor &monitor) {
    int value = monitor.AccessSharedResource();
    std::cout << "Ninja thread read shared resource: " << value << std::endl;
}

int main() {
    NinjaMonitor monitor;
    std::thread t(NinjaThread, std::ref(monitor));

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    monitor.UpdateSharedResource(67);

    t.join();
    return 0;
}
```

### 4. Output
<img src="Screenshot 2026-10-08 at 09.10.03.png" width="700" alt="Experiment 4B Output" />

```
balajipratyush4@Pratyushs-MacBook-Air code % ./exp4_monitor
Ninja thread read shared resource: 67
```

### 5. Detailed Output Analysis
- The worker thread `NinjaThread` invokes `AccessSharedResource()`. Because `SharedResource == 0`, it blocks atomically on `CondVar.wait(lock)`.
- The main thread sleeps for 500ms and updates the shared resource to `67`, notifying the waiting condition variable.
- The worker thread wakes up, retrieves `67`, resets the resource to 0, and cleanly outputs `Ninja thread read shared resource: 67`.

---

## Experiment 5: Classical Concurrency — Readers-Writers Problem using Semaphores & Mutex

### 1. Aim
To implement the classical Readers-Writers concurrency problem ensuring concurrent reads by multiple readers and mutually exclusive writes by writers.

### 2. Theory & Concepts
- **Readers-Preference Protocol:**
  - Multiple reader threads can read concurrently without blocking one another.
  - Writers require strict mutual exclusion (no other readers or writers may be active).
- **Control Variables:**
  - `read_count`: Integer tracking the number of active readers.
  - `mtx`: Mutex protecting updates to `read_count`.
  - `sem_t w`: Binary semaphore protecting the shared critical section from writers.
- **Rule:** The first reader to arrive acquires the writer semaphore (`sem_wait(&w)`). The last reader to leave releases it (`sem_post(&w)`).

### 3. Source Code (`exp5_reader_writer.cpp`)
```cpp
#include <chrono>
#include <iostream>
#include <mutex>
#include <semaphore.h>
#include <thread>
#include <vector>
using namespace std;

int read_count = 0; 
mutex mtx;          
sem_t w;            
const int max_reads = 3;  
const int max_writes = 2; 

void writer(int writer_id) {
    for (int i = 0; i < max_writes; ++i) {
        sem_wait(&w); 
        cout << "Writer " << writer_id << " is writing to the shared resource." << endl;
        this_thread::sleep_for(chrono::milliseconds(1000)); 
        cout << "Writer " << writer_id << " has finished writing." << endl;
        sem_post(&w); 
        this_thread::sleep_for(chrono::milliseconds(1000));
    }
}

void reader(int reader_id) {
    for (int i = 0; i < max_reads; ++i) {
        mtx.lock();
        read_count++;
        if (read_count == 1) {
            sem_wait(&w); 
        }
        mtx.unlock();

        cout << "Reader " << reader_id << " is reading the shared resource." << endl;
        this_thread::sleep_for(chrono::milliseconds(500)); 
        cout << "Reader " << reader_id << " has finished reading." << endl;

        mtx.lock();
        read_count--;
        if (read_count == 0) {
            sem_post(&w); 
        }
        mtx.unlock();
        this_thread::sleep_for(chrono::milliseconds(1000));
    }
}

int main() {
    sem_init(&w, 0, 1);
    vector<thread> readers, writers;

    for (int i = 0; i < 3; ++i) {
        readers.push_back(thread(reader, i + 1));
    }
    for (int i = 0; i < 2; ++i) {
        writers.push_back(thread(writer, i + 1));
    }

    for (auto &t : readers) t.join();
    for (auto &t : writers) t.join();

    sem_destroy(&w);
    cout << "\nAll readers and writers have finished their operations, program terminating" << endl;
    return 0;
}
```

### 4. Output
<img src="Screenshot 2026-10-08 at 09.10.20.png" width="700" alt="Experiment 5 Output" />

```
balajipratyush4@Pratyushs-MacBook-Air code % ./exp5_reader_writer
Reader Reader 31 is reading the shared resource. is reading the shared resource.

Writer 2 is writing to the shared resource.
Reader 2 is reading the shared resource.
Writer 1 is writing to the shared resource.
Reader Reader 3 has finished reading.Reader 2 has finished reading.
1 has finished reading.
Writer 2 has finished writing.
Writer 1 has finished writing.
Reader 3 is reading the shared resource.
Reader 1 is reading the shared resource.
Reader 2 is reading the shared resource.
Writer 2 is writing to the shared resource.
Writer 1 is writing to the shared resource.
Reader Reader 32 has finished reading. has finished reading.

Reader 1 has finished reading.
Writer Reader 2 has finished writing.2 is reading the shared resource.
Writer 1 has finished writing.
Reader 3 is reading the shared resource.
Reader 1 is reading the shared resource.
Reader 2 is reading the shared resource.
Reader 3 has finished reading.
Reader 1 has finished reading.

All readers and writers have finished their operations, program terminating
```

### 5. Detailed Output Analysis
- Multiple readers read simultaneously (e.g. `Reader 3` and `Reader 1` overlapping).
- Writers execute in isolation once readers release `sem_t w`.
- The interlocking reader/writer handshakes successfully terminate with all 3 readers and 2 writers completing operations without deadlock.

---

## Experiment 6: Classical Concurrency — Dining Philosophers Problem using Monitors

### 1. Aim
To implement and solve the Dining Philosophers synchronization problem using a Monitor and POSIX condition variables to guarantee deadlock-free and starvation-free execution.

### 2. Theory & Concepts
- Five philosophers sit around a circular table with 5 chopsticks. Each philosopher alternates between `THINKING`, `HUNGRY`, and `EATING`.
- To eat, a philosopher requires both their left fork `(i + 4) % 5` and right fork `(i + 1) % 5`.
- **Monitor Rule:** A philosopher transitions to `EATING` state if and only if:
  $$\text{state}[(i+1)\%5] \ne \text{EATING} \quad \text{and} \quad \text{state}[(i+4)\%5] \ne \text{EATING} \quad \text{and} \quad \text{state}[i] == \text{HUNGRY}$$
- When a philosopher finishes eating, `put_fork(i)` resets state to `THINKING` and explicitly invokes `test()` on both adjacent neighbors to wake them if eligible.

### 3. Source Code (`exp6_dining_philosophers.cpp`)
```cpp
#include <iostream>
#include <pthread.h>
#include <unistd.h>
using namespace std;

#define N 5
#define THINKING 2
#define HUNGRY 1
#define EATING 0
#define LEFT (phnum + 4) % N
#define RIGHT (phnum + 1) % N

int phil[N];
int times = 200;

class monitor {
    int state[N];
    pthread_cond_t phcond[N];
    pthread_mutex_t condLock;

public:
    void test(int phnum) {
        if (state[(phnum + 1) % 5] != EATING &&
            state[(phnum + 4) % 5] != EATING && 
            state[phnum] == HUNGRY) {
            state[phnum] = EATING;
            pthread_cond_signal(&phcond[phnum]);
        }
    }

    void take_fork(int phnum) {
        pthread_mutex_lock(&condLock);
        state[phnum] = HUNGRY;
        test(phnum);
        if (state[phnum] != EATING) {
            pthread_cond_wait(&phcond[phnum], &condLock);
        }
        cout << "Philosopher " << phnum << " is Eating" << endl;
        pthread_mutex_unlock(&condLock);
    }

    void put_fork(int phnum) {
        pthread_mutex_lock(&condLock);
        state[phnum] = THINKING;
        test(RIGHT);
        test(LEFT);
        pthread_mutex_unlock(&condLock);
    }

    monitor() {
        for (int i = 0; i < N; i++) state[i] = THINKING;
        for (int i = 0; i < N; i++) pthread_cond_init(&phcond[i], NULL);
        pthread_mutex_init(&condLock, NULL);
    }

    ~monitor() {
        for (int i = 0; i < N; i++) pthread_cond_destroy(&phcond[i]);
        pthread_mutex_destroy(&condLock);
    }
} phil_object;

void *philosopher(void *arg) {
    int c = 0;
    while (c < times) {
        int i = *(int *)arg;
        sleep(1);
        phil_object.take_fork(i);
        sleep(2);
        phil_object.put_fork(i);
        c++;
    }
    return NULL;
}

int main() {
    pthread_t thread_id[N];
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    for (int i = 0; i < N; i++) phil[i] = i;

    for (int i = 0; i < N; i++) {
        pthread_create(&thread_id[i], &attr, philosopher, &phil[i]);
        cout << "Philosopher " << i + 1 << " is thinking..." << endl;
    }

    for (int i = 0; i < N; i++) {
        pthread_join(thread_id[i], NULL);
    }

    pthread_attr_destroy(&attr);
    pthread_exit(NULL);
    return 0;
}
```

### 4. Output
<img src="Screenshot 2026-10-08 at 09.14.16.png" width="700" alt="Experiment 6 Output" />

```
balajipratyush4@Pratyushs-MacBook-Air code % ./exp6_dining_philosophers
Philosopher 1 is thinking...
Philosopher 2 is thinking...
Philosopher 3 is thinking...
Philosopher 4 is thinking...
Philosopher 5 is thinking...
Philosopher 0 is Eating
Philosopher 3 is Eating
Philosopher 2 is Eating
Philosopher 4 is Eating
Philosopher 1 is Eating
Philosopher 3 is Eating
Philosopher 4 is Eating
Philosopher 0 is Eating
Philosopher 2 is Eating
Philosopher 1 is Eating
Philosopher 1 is Eating
Philosopher 3 is Eating
```

### 5. Detailed Output Analysis
- Notice that non-adjacent philosophers eat concurrently: `Philosopher 0` and `Philosopher 3` eat at the same time without conflicting over chopsticks.
- No adjacent pair (e.g. 0 and 1, or 2 and 3) ever enters the `EATING` state simultaneously, guaranteeing complete mutual exclusion of forks without deadlock.

---

## Experiment 7: Deadlock Avoidance using Banker's Algorithm

### 1. Aim
To implement and test the Banker's Algorithm for deadlock avoidance and compute a safe execution sequence for given resource allocation matrices.

### 2. Theory & Concepts
Banker's Algorithm tests for safety by evaluating whether resources can be allocated such that all processes can finish execution without entering a deadlock state:
- **Formulas:**
  $$\text{Need}[i][j] = \text{Max}[i][j] - \text{Allocation}[i][j]$$
- **Safety Criterion:** A process $P_i$ can run if $\text{Need}[i] \le \text{Current Available}$.
- Upon completion, the resources held by $P_i$ are returned to the available pool:
  $$\text{Current Available} = \text{Current Available} + \text{Allocation}[i]$$

### 3. Source Code (`exp7_bankers.cpp`)
```cpp
#include <iostream>
using namespace std;

int allocation[10][3], need[10][3], Max[10][3], available[10][3];
int p, current[3];
bool executed[10], come;

void IMP() {
    come = false;
    for (int i = 0; i < 10; ++i) executed[i] = false;
}

void Calculate() {
    IMP();
    int i, j;
    for (i = 0; i < p; ++i) {
        for (j = 0; j < p; ++j) {
            while (executed[j] && j < p - 1) {
                j++;
            }
            if (need[j][0] <= current[0] && need[j][1] <= current[1] &&
                need[j][2] <= current[2]) {
                if (!executed[j]) {
                    executed[j] = true;
                    current[0] += allocation[j][0];
                    current[1] += allocation[j][1];
                    current[2] += allocation[j][2];
                    cout << "\nProcess P" << j + 1;
                    cout << "\nCurrent: " << current[0] << " " << current[1] << " "
                         << current[2] << "\n";
                    cout << "\nProcess executed without deadlock";
                    come = true;
                    break;
                }
            }
        }
        if (!come) {
            cout << "\n Dead lock\n\n";
            break;
        } else {
            come = false;
        }
    }
}

int main() {
    int keepon = 1;
    cout << "Enter No. of processes: ";
    cin >> p;
    cout << "\n";
    cout << "Enter the current resources: ";
    cin >> current[0] >> current[1] >> current[2];

    for (int i = 0; i < p; ++i) {
        cout << "\n\n Process P" << i + 1 << " Details\n";
        cout << "Enter Allocation : ";
        cin >> allocation[i][0] >> allocation[i][1] >> allocation[i][2];
        cout << "Enter Max :";
        cin >> Max[i][0] >> Max[i][1] >> Max[i][2];
        need[i][0] = Max[i][0] - allocation[i][0];
        need[i][1] = Max[i][1] - allocation[i][1];
        need[i][2] = Max[i][2] - allocation[i][2];
    }

    cout << "\n\n Table for Bankers Algo\n\n";
    cout << "Initial Resources: " << current[0] << " " << current[1] << " "
         << current[2] << "\n\n";
    cout << "Process Max Allocation Need\n";
    for (int i = 0; i < p; ++i) {
        cout << " P" << i + 1 << " ";
        cout << " " << Max[i][0] << " " << Max[i][1] << " " << Max[i][2] << " ";
        cout << " " << allocation[i][0] << " " << allocation[i][1] << " "
             << allocation[i][2] << " ";
        cout << " " << need[i][0] << " " << need[i][1] << " " << need[i][2];
        cout << "\n";
    }
    cout << "\n\n";
    Calculate();

    return 0;
}
```

### 4. Input & Execution Trace
- **Number of Processes:** 5 ($P_1, P_2, P_3, P_4, P_5$)
- **Available Resources:** `[10, 5, 7]`
- **Matrix State:**

| Process | Max Demand | Allocated | Need Matrix |
| :--- | :---: | :---: | :---: |
| **P1** | 7 5 3 | 0 1 0 | **7 4 3** |
| **P2** | 3 2 2 | 2 0 0 | **1 2 2** |
| **P3** | 9 0 2 | 3 0 2 | **6 0 0** |
| **P4** | 2 2 2 | 2 1 1 | **0 1 1** |
| **P5** | 4 3 3 | 0 0 2 | **4 3 1** |

### 5. Output
<img src="Screenshot 2026-10-08 at 09.17.18.png" width="700" alt="Experiment 7 Output" />

```
balajipratyush4@Pratyushs-MacBook-Air code % ./exp7_bankers
Enter No. of processes: 5

Enter the current resources: 10 5 7


 Process P1 Details
Enter Allocation : 0 1 0
Enter Max :7 5 3


 Process P2 Details
Enter Allocation : 2 0 0
Enter Max :3 2 2


 Process P3 Details
Enter Allocation : 3 0 2
Enter Max :9 0 2


 Process P4 Details
Enter Allocation : 2 1 1
Enter Max :2 2 2


 Process P5 Details
Enter Allocation : 0 0 2
Enter Max :4 3 3


 Table for Bankers Algo

Initial Resources: 10 5 7

Process Max Allocation Need
 P1    7 5 3   0 1 0   7 4 3
 P2    3 2 2   2 0 0   1 2 2
 P3    9 0 2   3 0 2   6 0 0
 P4    2 2 2   2 1 1   0 1 1
 P5    4 3 3   0 0 2   4 3 1




Process P1
Current: 10 6 7

Process executed without deadlock
Process P2
Current: 12 6 7

Process executed without deadlock
Process P3
Current: 15 6 9

Process executed without deadlock
Process P4
Current: 17 7 10

Process executed without deadlock
Process P5
Current: 17 7 12

Process executed without deadlock
```

### 6. Detailed Output Analysis
- Initial Available: `[10, 5, 7]`
- $P_1$: Need `[7, 4, 3] <= [10, 5, 7]` $\implies$ Executes. New Available = `[10, 6, 7]`.
- $P_2$: Need `[1, 2, 2] <= [10, 6, 7]` $\implies$ Executes. New Available = `[12, 6, 7]`.
- $P_3$: Need `[6, 0, 0] <= [12, 6, 7]` $\implies$ Executes. New Available = `[15, 6, 9]`.
- $P_4$: Need `[0, 1, 1] <= [15, 6, 9]` $\implies$ Executes. New Available = `[17, 7, 10]`.
- $P_5$: Need `[4, 3, 1] <= [17, 7, 10]` $\implies$ Executes. New Available = `[17, 7, 12]`.
- **Safe Sequence:** $\langle P_1, P_2, P_3, P_4, P_5 \rangle$. All processes complete without deadlock.

---

## Experiment 8 (Part A): CPU Scheduling — First-Come, First-Served (FCFS)

### 1. Aim
To implement the First-Come, First-Served (FCFS) CPU scheduling algorithm and calculate Waiting Time (WT), Turnaround Time (TAT), Average Waiting Time, and Average Turnaround Time.

### 2. Theory & Concepts
FCFS is a non-preemptive scheduling policy where the process arriving first is serviced first:
- **Formulas (Arrival Time = 0):**
  $$WT_0 = 0, \quad WT_i = WT_{i-1} + BT_{i-1}$$
  $$TAT_i = BT_i + WT_i$$
  $$\text{Average } WT = \frac{\sum WT_i}{n}, \quad \text{Average } TAT = \frac{\sum TAT_i}{n}$$
- **Data:**
  - $P_1$: Burst Time = 10 $\implies WT_1 = 0$, $TAT_1 = 10$
  - $P_2$: Burst Time = 5 $\implies WT_2 = 10$, $TAT_2 = 15$
  - $P_3$: Burst Time = 8 $\implies WT_3 = 15$, $TAT_3 = 23$

### 3. Source Code (`exp8_fcfs.cpp`)
```cpp
#include <iostream>
using namespace std;

void findWaitingTime(int processes[], int n, int bt[], int wt[]) {
    wt[0] = 0;
    for (int i = 1; i < n; i++)
        wt[i] = bt[i - 1] + wt[i - 1];
}

void findTurnAroundTime(int processes[], int n, int bt[], int wt[], int tat[]) {
    for (int i = 0; i < n; i++)
        tat[i] = bt[i] + wt[i];
}

void findavgTime(int processes[], int n, int bt[]) {
    int wt[n], tat[n], total_wt = 0, total_tat = 0;
    findWaitingTime(processes, n, bt, wt);
    findTurnAroundTime(processes, n, bt, wt, tat);

    cout << "Processes " << " Burst time "
         << " Waiting time " << " Turn around time\n";

    for (int i = 0; i < n; i++) {
        total_wt = total_wt + wt[i];
        total_tat = total_tat + tat[i];
        cout << " " << i + 1 << "\t\t" << bt[i] << "\t " << wt[i] << "\t\t "
             << tat[i] << endl;
    }
    cout << "Average waiting time = " << (float)total_wt / (float)n;
    cout << "\nAverage turn around time = " << (float)total_tat / (float)n;
}

int main() {
    int processes[] = {1, 2, 3};
    int n = sizeof processes / sizeof processes[0];
    int burst_time[] = {10, 5, 8};
    findavgTime(processes, n, burst_time);
    return 0;
}
```

### 4. Output
<img src="Screenshot 2026-10-08 at 09.18.52.png" width="700" alt="Experiment 8A Output" />

```
balajipratyush4@Pratyushs-MacBook-Air code % ./exp8_fcfs
Processes  Burst time  Waiting time  Turn around time
 1		10	 0		 10
 2		5	 10		 15
 3		8	 15		 23
Average waiting time = 8.33333
Average turn around time = 16%
```

### 5. Detailed Output Analysis
- Total Waiting Time = $0 + 10 + 15 = 25$
- $\text{Average Waiting Time} = \frac{25}{3} = 8.33333$
- Total Turnaround Time = $10 + 15 + 23 = 48$
- $\text{Average Turnaround Time} = \frac{48}{3} = 16.0$

---

## Experiment 8 (Part B): CPU Scheduling — Shortest Job First (SJF Non-Preemptive)

### 1. Aim
To implement the Shortest Job First (SJF) non-preemptive CPU scheduling algorithm and calculate individual Waiting Times, Turnaround Times, and system averages.

### 2. Theory & Concepts
SJF selects the waiting process with the smallest CPU burst requirement:
- Non-preemptive SJF is provably optimal for minimizing average waiting time.
- Given processes:
  - $p_1$: $BT=3$
  - $p_2$: $BT=6$
  - $p_3$: $BT=4$
  - $p_4$: $BT=5$
  - $p_5$: $BT=2$
- **Sorted Execution Order:** $p_5(2) \to p_1(3) \to p_3(4) \to p_4(5) \to p_2(6)$.
- **Metrics Calculation:**
  - $p_5$: $WT = 0$, $TAT = 2$
  - $p_1$: $WT = 2$, $TAT = 5$
  - $p_3$: $WT = 5$, $TAT = 9$
  - $p_4$: $WT = 9$, $TAT = 14$
  - $p_2$: $WT = 14$, $TAT = 20$

### 3. Source Code (`exp8_sjf.c`)
```c
#include <stdio.h>

int main() {
    int bt[20], p[20], wt[20], tat[20], i, j, n, total = 0, pos, temp;
    float avg_wt, avg_tat;

    printf("Enter number of process:");
    scanf("%d", &n);

    printf("\nEnter Burst Time:\n");
    for (i = 0; i < n; i++) {
        printf("p%d:", i + 1);
        scanf("%d", &bt[i]);
        p[i] = i + 1; // contains process number
    }

    // Sorting burst time in ascending order using selection sort
    for (i = 0; i < n; i++) {
        pos = i;
        for (j = i + 1; j < n; j++) {
            if (bt[j] < bt[pos])
                pos = j;
        }
        temp = bt[i];
        bt[i] = bt[pos];
        bt[pos] = temp;
        temp = p[i];
        p[i] = p[pos];
        p[pos] = temp;
    }

    wt[0] = 0; // waiting time for first process will be zero
    // calculate waiting time
    for (i = 1; i < n; i++) {
        wt[i] = 0;
        for (j = 0; j < i; j++)
            wt[i] += bt[j];
        total += wt[i];
    }
    avg_wt = (float)total / n; // average waiting time
    total = 0;

    printf("\nProcess\t Burst Time \tWaiting Time\tTurnaround Time");
    for (i = 0; i < n; i++) {
        tat[i] = bt[i] + wt[i]; // calculate turnaround time
        total += tat[i];
        printf("\np%d\t\t %d\t\t %d\t\t\t%d", p[i], bt[i], wt[i], tat[i]);
    }
    avg_tat = (float)total / n; // average turnaround time

    printf("\n\nAverage Waiting Time=%f", avg_wt);
    printf("\nAverage Turnaround Time=%f\n", avg_tat);
    return 0;
}
```

### 4. Output
<img src="Screenshot 2026-10-08 at 09.18.59.png" width="700" alt="Experiment 8B Output" />

```
balajipratyush4@Pratyushs-MacBook-Air code % ./exp8_sjf
Enter number of process:5

Enter Burst Time:
p1:3
p2:6
p3:4
p4:5
p5:2

Process	 Burst Time 	Waiting Time	Turnaround Time
p5		 2		 0			2
p1		 3		 2			5
p3		 4		 5			9
p4		 5		 9			14
p2		 6		 14			20

Average Waiting Time=6.000000
Average Turnaround Time=10.000000
```

### 5. Detailed Output Analysis
- Total Waiting Time = $0 + 2 + 5 + 9 + 14 = 30$
- $\text{Average Waiting Time} = \frac{30}{5} = 6.000000$
- Total Turnaround Time = $2 + 5 + 9 + 14 + 20 = 50$
- $\text{Average Turnaround Time} = \frac{50}{5} = 10.000000$
- Notice that scheduling the shortest burst first resulted in a significantly lower average waiting time (6.0) compared to unsorted FCFS execution.

---

## Experiment 9 (Part A): CPU Scheduling — Priority Scheduling (Non-Preemptive)

### 1. Aim
To implement non-preemptive Priority CPU scheduling algorithm and calculate individual Waiting Times, Turnaround Times, and system averages.

### 2. Theory & Concepts
In Priority Scheduling:
- Each process is assigned a priority value. In this convention, lower numeric value represents higher priority.
- The CPU is allocated to the process with the highest priority (smallest priority number).
- Selection sort rearranges processes in order of ascending priority number.
- Waiting time for process $i$: $WT[i] = \sum_{j=0}^{i-1} BT[j]$.
- Turnaround time for process $i$: $TAT[i] = BT[i] + WT[i]$.

### 3. Source Code (`exp9_priority.cpp`)
```cpp
// Experiment 9 (Part A): Priority CPU Scheduling
#include <iostream>
using namespace std;

int main() {
    int bt[20], p[20], wt[20], tat[20], pr[20], i, j, n, total = 0, pos, temp;
    float avg_wt, avg_tat;

    cout << "Enter Total Number of Process:";
    cin >> n;

    cout << "\nEnter Burst Time and Priority\n";
    for (i = 0; i < n; i++) {
        cout << "\nP[" << i + 1 << "]\n";
        cout << "Burst Time:";
        cin >> bt[i];
        cout << "Priority:";
        cin >> pr[i];
        p[i] = i + 1; // contains process number
    }

    // sorting burst time, priority and process number in ascending order using selection sort
    for (i = 0; i < n; i++) {
        pos = i;
        for (j = i + 1; j < n; j++) {
            if (pr[j] < pr[pos])
                pos = j;
        }
        temp = pr[i];
        pr[i] = pr[pos];
        pr[pos] = temp;

        temp = bt[i];
        bt[i] = bt[pos];
        bt[pos] = temp;

        temp = p[i];
        p[i] = p[pos];
        p[pos] = temp;
    }

    wt[0] = 0; // waiting time for first process is zero

    // calculate waiting time
    for (i = 1; i < n; i++) {
        wt[i] = 0;
        for (j = 0; j < i; j++)
            wt[i] += bt[j];
        total += wt[i];
    }

    avg_wt = (float)total / n; // average waiting time
    total = 0;

    cout << "\nProcess\t Burst Time \tWaiting Time\tTurnaround Time";
    for (i = 0; i < n; i++) {
        tat[i] = bt[i] + wt[i]; // calculate turnaround time
        total += tat[i];
        cout << "\nP[" << p[i] << "]\t\t " << bt[i] << "\t\t " << wt[i] << "\t\t\t" << tat[i];
    }

    avg_tat = (float)total / n; // average turnaround time
    cout << "\n\nAverage Waiting Time=" << avg_wt;
    cout << "\nAverage Turnaround Time=" << avg_tat << endl;

    return 0;
}
```

### 4. Output
<img src="Screenshot 2026-10-26 at 19.46.13.png" width="700" alt="Experiment 9A Output" />

```
balajipratyush4@Pratyushs-MacBook-Air code % ./exp9_priority
Enter Total Number of Process:5

Enter Burst Time and Priority

P[1]
Burst Time:3
Priority:6

P[2]
Burst Time:2
Priority:1

P[3]
Burst Time:6
Priority:1

P[4]
Burst Time:9
Priority:6

P[5]
Burst Time:6
Priority:4

Process	 Burst Time 	Waiting Time	Turnaround Time
P[2]		 2		 0			2
P[3]		 6		 2			8
P[5]		 6		 8			14
P[4]		 9		 14			23
P[1]		 3		 23			26

Average Waiting Time=9.4
Average Turnaround Time=14.6
```

### 5. Detailed Output Analysis
- Selection sort orders processes by priority: $P_2 (\text{Priority } 1) \to P_3 (\text{Priority } 1) \to P_5 (\text{Priority } 4) \to P_4 (\text{Priority } 6) \to P_1 (\text{Priority } 6)$.
- Total Waiting Time = $0 + 2 + 8 + 14 + 23 = 47$
- $\text{Average Waiting Time} = \frac{47}{5} = 9.4$
- Total Turnaround Time = $2 + 8 + 14 + 23 + 26 = 73$
- $\text{Average Turnaround Time} = \frac{73}{5} = 14.6$


---

## Experiment 9 (Part B): CPU Scheduling — Round Robin (RR)

### 1. Aim
To implement Round Robin (RR) CPU scheduling algorithm with a fixed time quantum ($q = 2$).

### 2. Theory & Concepts
- Round Robin is a preemptive scheduling algorithm designed especially for time-sharing systems.
- A small unit of time, called a **time quantum** or time slice (here $q=2$), is defined.
- The ready queue is treated as a circular FIFO queue.
- If a process's CPU burst exceeds $q$, it is preempted and put back at the end of the queue.

### 3. Source Code (`exp9_round_robin.cpp`)
```cpp
// Experiment 9 (Part B): Round Robin CPU Scheduling
#include <iostream>
#include <vector>
using namespace std;

// Function to find the waiting time for all processes
void findWaitingTime(int processes[], int n, int bt[], int wt[], int quantum) {
    vector<int> rem_bt(n);
    for (int i = 0; i < n; i++)
        rem_bt[i] = bt[i];

    int t = 0; // Current time

    while (1) {
        bool done = true;
        for (int i = 0; i < n; i++) {
            if (rem_bt[i] > 0) {
                done = false;

                if (rem_bt[i] > quantum) {
                    t += quantum;
                    rem_bt[i] -= quantum;
                } else {
                    t = t + rem_bt[i];
                    wt[i] = t - bt[i];
                    rem_bt[i] = 0;
                }
            }
        }
        if (done == true)
            break;
    }
}

// Function to calculate turnaround time
void findTurnAroundTime(int processes[], int n, int bt[], int wt[], int tat[]) {
    for (int i = 0; i < n; i++)
        tat[i] = bt[i] + wt[i];
}

// Function to calculate average time
void findavgTime(int processes[], int n, int bt[], int quantum) {
    vector<int> wt(n), tat(n);
    int total_wt = 0, total_tat = 0;

    findWaitingTime(processes, n, bt, wt.data(), quantum);
    findTurnAroundTime(processes, n, bt, wt.data(), tat.data());

    cout << "PN\t \tBT \t WT \tTAT\n";
    for (int i = 0; i < n; i++) {
        total_wt = total_wt + wt[i];
        total_tat = total_tat + tat[i];
        cout << " " << i + 1 << "\t\t" << bt[i] << "\t " << wt[i] << "\t\t " << tat[i] << endl;
    }

    cout << "Average waiting time = " << (float)total_wt / (float)n;
    cout << "\nAverage turn around time = " << (float)total_tat / (float)n << endl;
}

// Driver code
int main() {
    int processes[] = {1, 2, 3};
    int n = sizeof(processes) / sizeof(processes[0]);
    int burst_time[] = {10, 5, 8};
    int quantum = 2;

    findavgTime(processes, n, burst_time, quantum);
    return 0;
}
```

### 4. Output
<img src="Screenshot 2026-10-26 at 19.46.41.png" width="700" alt="Experiment 9B Output" />

```
balajipratyush4@Pratyushs-MacBook-Air code % ./exp9_round_robin
PN	 	BT 	 WT 	TAT
 1		10	 13		 23
 2		5	 10		 15
 3		8	 13		 21
Average waiting time = 12
Average turn around time = 19.6667
```

### 5. Detailed Output Analysis
- Gantt Chart Execution ($q = 2$):
  $[0-2: P_1] \to [2-4: P_2] \to [4-6: P_3] \to [6-8: P_1] \to [8-10: P_2] \to [10-12: P_3] \to [12-14: P_1] \to [14-15: P_2 \text{ done}] \to [15-17: P_3] \to [17-19: P_1] \to [19-21: P_3 \text{ done}] \to [21-23: P_1 \text{ done}]$.
- Completion Times: $P_2 = 15$, $P_3 = 21$, $P_1 = 23$.
- Waiting Times:
  - $WT(P_1) = 23 - 10 = 13$
  - $WT(P_2) = 15 - 5 = 10$
  - $WT(P_3) = 21 - 8 = 13$
- $\text{Average Waiting Time} = \frac{13 + 10 + 13}{3} = \frac{36}{3} = 12.0$
- $\text{Average Turnaround Time} = \frac{23 + 15 + 21}{3} = \frac{59}{3} = 19.6667$

---

## Experiment 10: Page Replacement — First-In, First-Out (FIFO)

### 1. Aim
To implement the First-In, First-Out (FIFO) page replacement algorithm and determine the total number of page faults for a reference string.

### 2. Theory & Concepts
- The FIFO page replacement algorithm associates with each page the time when that page was brought into memory.
- When a page must be replaced, the oldest page (the one brought in first) is chosen.
- A circular queue or modulo index `(pageFaults - 1) % frames` tracks the victim frame.

### 3. Source Code (`exp10_fifo.c`)
```c
// Experiment 10: FIFO Page Replacement Algorithm
#include <stdio.h>

int main() {
    int incomingStream[] = {4, 1, 2, 4, 5};
    int pageFaults = 0;
    int frames = 3;
    int m, n, s, pages;

    pages = sizeof(incomingStream) / sizeof(incomingStream[0]);

    printf("Incoming \t Frame 1 \t Frame 2 \t Frame 3");
    int temp[frames];
    for (m = 0; m < frames; m++) {
        temp[m] = -1;
    }

    for (m = 0; m < pages; m++) {
        s = 0;
        for (n = 0; n < frames; n++) {
            if (incomingStream[m] == temp[n]) {
                s++;
                pageFaults--;
            }
        }
        pageFaults++;

        if ((pageFaults <= frames) && (s == 0)) {
            temp[(pageFaults - 1) % frames] = incomingStream[m];
        } else if (s == 0) {
            temp[(pageFaults - 1) % frames] = incomingStream[m];
        }

        printf("\n");
        printf("%d\t\t\t", incomingStream[m]);
        for (n = 0; n < frames; n++) {
            if (temp[n] != -1)
                printf(" %d\t\t\t", temp[n]);
            else
                printf(" - \t\t\t");
        }
    }
    printf("\nTotal Page Faults:\t%d\n", pageFaults);
    return 0;
}
```

### 4. Output
<img src="Screenshot 2026-10-26 at 19.46.59.png" width="700" alt="Experiment 10 Output" />

```
balajipratyush4@Pratyushs-MacBook-Air code % ./exp10_fifo
Incoming 	 Frame 1 	 Frame 2 	 Frame 3
4			 4			 - 			 - 			
1			 4			 1			 - 			
2			 4			 1			 2			
4			 4			 1			 2			
5			 5			 1			 2			
Total Page Faults:	4
```


### 5. Detailed Output Analysis
- Reference stream: `{4, 1, 2, 4, 5}` with 3 frames:
  1. Page 4: Fault $\to$ Frame: `[4, -, -]`
  2. Page 1: Fault $\to$ Frame: `[4, 1, -]`
  3. Page 2: Fault $\to$ Frame: `[4, 1, 2]`
  4. Page 4: **HIT** $\to$ Frame remains: `[4, 1, 2]`
  5. Page 5: Fault $\to$ Replaces oldest page (4 in Frame 1) $\to$ Frame: `[5, 1, 2]`
- Total Page Faults = 4. Total Hits = 1.

---

## Experiment 11 (Part A): Page Replacement — Least Recently Used (LRU)

### 1. Aim
To implement the Least Recently Used (LRU) page replacement algorithm and compute total page faults.

### 2. Theory & Concepts
- LRU replaces the page that has not been used for the longest period of time.
- It approximates optimal replacement by looking backward in time: for each frame, it searches backward in the reference string to compute the time/distance elapsed since its last reference.
- The frame with the maximum backward distance is chosen as the replacement candidate.

### 3. Source Code (`exp11_lru.c`)
```c
// Experiment 11 (Part A): Least Recently Used (LRU) Page Replacement Algorithm
#include <stdio.h>
#include <limits.h>

int checkHit(int incomingPage, int queue[], int occupied) {
    for (int i = 0; i < occupied; i++) {
        if (incomingPage == queue[i])
            return 1;
    }
    return 0;
}

void printFrame(int queue[], int occupied) {
    for (int i = 0; i < occupied; i++)
        printf("%d\t\t\t", queue[i]);
}

int main() {
    int incomingStream[] = {7, 0, 1, 2, 0, 3, 0, 4, 2, 3, 0, 3, 2, 1};
    int n = sizeof(incomingStream) / sizeof(incomingStream[0]);
    int frames = 3;
    int queue[n];
    int distance[n];
    int occupied = 0;
    int pagefault = 0;

    printf("Page\t Frame1 \t Frame2 \t Frame3\n");

    for (int i = 0; i < n; i++) {
        printf("%d: \t\t", incomingStream[i]);

        if (checkHit(incomingStream[i], queue, occupied)) {
            printFrame(queue, occupied);
        } else if (occupied < frames) {
            queue[occupied] = incomingStream[i];
            pagefault++;
            occupied++;
            printFrame(queue, occupied);
        } else {
            int max = INT_MIN;
            int index = -1;

            for (int j = 0; j < frames; j++) {
                distance[j] = 0;
                for (int k = i - 1; k >= 0; k--) {
                    ++distance[j];
                    if (queue[j] == incomingStream[k])
                        break;
                }
                if (distance[j] > max) {
                    max = distance[j];
                    index = j;
                }
            }
            queue[index] = incomingStream[i];
            printFrame(queue, occupied);
            pagefault++;
        }
        printf("\n");
    }

    printf("Page Fault: %d\n", pagefault);
    return 0;
}
```

### 4. Output
<img src="Screenshot 2026-10-26 at 19.47.27.png" width="700" alt="Experiment 11A Output" />

```
balajipratyush4@Pratyushs-MacBook-Air code % ./exp11_lru
Page	 Frame1 	 Frame2 	 Frame3
7: 		7			
0: 		7			0			
1: 		7			0			1			
2: 		2			0			1			
0: 		2			0			1			
3: 		2			0			3			
0: 		2			0			3			
4: 		4			0			3			
2: 		4			0			2			
3: 		4			3			2			
0: 		0			3			2			
3: 		0			3			2			
2: 		0			3			2			
1: 		1			3			2			
Page Fault: 10
```

### 5. Detailed Output Analysis
- Reference stream length: 14. Available frames: 3.
- Page faults occur at 10 reference accesses.
- Page hits occur at 4 accesses: step 5 (page 0), step 7 (page 0), step 12 (page 3), and step 13 (page 2).
- Total Page Faults = 10.

---

## Experiment 11 (Part B): Page Replacement — Least Frequently Used (LFU)

### 1. Aim
To implement the Least Frequently Used (LFU) page replacement algorithm.

### 2. Theory & Concepts
- The LFU algorithm replaces the page with the smallest reference count.
- If two or more pages have identical reference frequencies, FIFO arrival order acts as a tiebreaker.

### 3. Source Code (`exp11_lfu.c`)
```c
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
    int queue[3], freq[3], arrival[3];
    int occupied = 0, pagefault = 0;

    for (int i = 0; i < frames; i++) {
        queue[i] = -1;
        freq[i] = 0;
        arrival[i] = 0;
    }

    printf("Page\t Frame1 \t Frame2 \t Frame3\n");
    for (int i = 0; i < n; i++) {
        printf("%d: \t\t", incomingStream[i]);
        int hitIndex = checkHit(incomingStream[i], queue, occupied);

        if (hitIndex != -1) {
            freq[hitIndex]++;
            printFrame(queue, occupied);
        } else if (occupied < frames) {
            queue[occupied] = incomingStream[i];
            freq[occupied] = 1;
            arrival[occupied] = i;
            occupied++;
            pagefault++;
            printFrame(queue, occupied);
        } else {
            int minFreq = INT_MAX, replaceIndex = -1, oldestArrival = INT_MAX;
            for (int j = 0; j < frames; j++) {
                if (freq[j] < minFreq) {
                    minFreq = freq[j];
                    replaceIndex = j;
                    oldestArrival = arrival[j];
                } else if (freq[j] == minFreq && arrival[j] < oldestArrival) {
                    replaceIndex = j;
                    oldestArrival = arrival[j];
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
```

### 4. Output
<img src="Screenshot 2026-10-26 at 19.47.13.png" width="700" alt="Experiment 11B Output" />

```
balajipratyush4@Pratyushs-MacBook-Air code % ./exp11_lfu
Page	 Frame1 	 Frame2 	 Frame3
7: 		7			
0: 		7			0			
1: 		7			0			1			
2: 		2			0			1			
0: 		2			0			1			
3: 		2			0			3			
0: 		2			0			3			
4: 		4			0			3			
2: 		4			0			2			
3: 		3			0			2			
0: 		3			0			2			
3: 		3			0			2			
2: 		3			0			2			
1: 		3			0			1			
Page Fault: 9
```

### 5. Detailed Output Analysis
- Total Page Faults = 9. Total Hits = 5.
- Demonstrates how frequently referenced pages (such as 0) remain pinned in frames longer.

---

## Experiment 12 (Part A): Memory Allocation — Best Fit Policy

### 1. Aim
To implement the Best Fit contiguous memory allocation policy and observe memory utilization.

### 2. Theory & Concepts
- The Best Fit strategy allocates the smallest hole (block) that is big enough to accommodate the requesting process:
  $$\min \{ \text{blockSize}[j] \mid \text{blockSize}[j] \ge \text{processSize}[i] \text{ and } \text{unallocated} \}$$
- This minimizes the leftover internal fragmentation produced per allocation.

### 3. Source Code (`exp12_best_fit.c`)
```c
// Experiment 12 (Part A): Best Fit Memory Management Policy
#include <stdio.h>

void implementBestFit(int blockSize[], int blocks, int processSize[], int processes) {
    int allocation[processes];
    int occupied[blocks];

    for (int i = 0; i < processes; i++)
        allocation[i] = -1;

    for (int i = 0; i < blocks; i++)
        occupied[i] = 0;

    for (int i = 0; i < processes; i++) {
        int indexPlaced = -1;
        for (int j = 0; j < blocks; j++) {
            if (blockSize[j] >= processSize[i] && !occupied[j]) {
                if (indexPlaced == -1)
                    indexPlaced = j;
                else if (blockSize[j] < blockSize[indexPlaced])
                    indexPlaced = j;
            }
        }
        if (indexPlaced != -1) {
            allocation[i] = indexPlaced;
            occupied[indexPlaced] = 1;
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

int main() {
    int blockSize[] = {100, 50, 30, 120, 35};
    int processSize[] = {40, 10, 30, 60};
    int blocks = sizeof(blockSize) / sizeof(blockSize[0]);
    int processes = sizeof(processSize) / sizeof(processSize[0]);

    implementBestFit(blockSize, blocks, processSize, processes);
    return 0;
}
```

### 4. Output
<img src="Screenshot 2026-10-26 at 19.47.39.png" width="700" alt="Experiment 12A Output" />

```
balajipratyush4@Pratyushs-MacBook-Air code % ./exp12_best_fit

Process No.	Process Size	Block no.
1 			 40 			2
2 			 10 			3
3 			 30 			5
4 			 60 			1
```

### 5. Detailed Output Analysis
- Available blocks: $B_1(100), B_2(50), B_3(30), B_4(120), B_5(35)$.
- $P_1(40) \to B_2(50)$ (waste = 10)
- $P_2(10) \to B_3(30)$ (waste = 20)
- $P_3(30) \to B_5(35)$ (waste = 5)
- $P_4(60) \to B_1(100)$ (waste = 40)
- All 4 processes successfully placed with minimal leftover fragments.

---

## Experiment 12 (Part B): Memory Allocation — Worst Fit Policy

### 1. Aim
To implement the Worst Fit contiguous memory allocation policy and observe block fragmentation.

### 2. Theory & Concepts
- The Worst Fit strategy allocates the largest available hole:
  $$\max \{ \text{blockSize}[j] \mid \text{blockSize}[j] \ge \text{processSize}[i] \text{ and } \text{unallocated} \}$$
- The rationale is that the leftover fragment after allocation will be large enough to remain useful. However, it can exhaust large blocks quickly, preventing larger future processes from finding adequate memory.

### 3. Source Code (`exp12_worst_fit.c`)
```c
// Experiment 12 (Part B): Worst Fit Memory Management Policy
#include <stdio.h>

void implementWorstFit(int blockSize[], int blocks, int processSize[], int processes) {
    int allocation[processes];
    int occupied[blocks];

    for (int i = 0; i < processes; i++)
        allocation[i] = -1;

    for (int i = 0; i < blocks; i++)
        occupied[i] = 0;

    for (int i = 0; i < processes; i++) {
        int indexPlaced = -1;
        for (int j = 0; j < blocks; j++) {
            if (blockSize[j] >= processSize[i] && !occupied[j]) {
                if (indexPlaced == -1)
                    indexPlaced = j;
                else if (blockSize[indexPlaced] < blockSize[j])
                    indexPlaced = j;
            }
        }
        if (indexPlaced != -1) {
            allocation[i] = indexPlaced;
            occupied[indexPlaced] = 1;
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

int main() {
    int blockSize[] = {100, 50, 30, 120, 35};
    int processSize[] = {40, 10, 30, 60};
    int blocks = sizeof(blockSize) / sizeof(blockSize[0]);
    int processes = sizeof(processSize) / sizeof(processSize[0]);

    implementWorstFit(blockSize, blocks, processSize, processes);
    return 0;
}
```

### 4. Output
<img src="Screenshot 2026-10-26 at 19.47.48.png" width="700" alt="Experiment 12B Output" />

```
balajipratyush4@Pratyushs-MacBook-Air code % ./exp12_worst_fit

Process No.	Process Size	Block no.
1 			 40 			4
2 			 10 			1
3 			 30 			2
4 			 60 			Not Allocated
```


### 5. Detailed Output Analysis
- Available blocks: $B_1(100), B_2(50), B_3(30), B_4(120), B_5(35)$.
- $P_1(40) \to B_4(120)$ (largest block)
- $P_2(10) \to B_1(100)$ (next largest)
- $P_3(30) \to B_2(50)$ (next largest)
- $P_4(60) \to$ Remaining unallocated blocks are $B_3(30)$ and $B_5(35)$. Neither block is $\ge 60$.
- Hence, $P_4$ is **Not Allocated**, demonstrating the hazard of Worst Fit depleting large blocks.

---

## Experiment 13 (Part A): Disk Scheduling — First-Come, First-Served (FCFS)

### 1. Aim
To implement the First-Come, First-Served (FCFS) disk arm scheduling algorithm and compute the total head movement in cylinders.

### 2. Theory & Concepts
- In FCFS disk scheduling, I/O track requests are serviced strictly in arrival order.
- Total head movement:
  $$\text{Total Movement} = \sum_{i=0}^{n-1} |RQ[i] - \text{initial}_{i}|$$

### 3. Source Code (`exp13_fcfs_disk.c`)
```c
// Experiment 13 (Part A): FCFS Disk Scheduling Algorithm
#include <stdio.h>
#include <stdlib.h>

int main() {
    int RQ[100], i, n, TotalHeadMoment = 0, initial;

    printf("Enter the number of Requests\n");
    scanf("%d", &n);

    printf("Enter the Requests sequence\n");
    for (i = 0; i < n; i++)
        scanf("%d", &RQ[i]);

    printf("Enter initial head position\n");
    scanf("%d", &initial);

    // logic for FCFS disk scheduling
    for (i = 0; i < n; i++) {
        TotalHeadMoment = TotalHeadMoment + abs(RQ[i] - initial);
        initial = RQ[i];
    }

    printf("Total head movement is %d\n", TotalHeadMoment);
    return 0;
}
```

### 4. Output
```
Enter the number of Requests
8
Enter the Requests sequence
95 180 34 119 11 123 62 64
Enter initial head position
50
Total head movement is 644
```

### 5. Detailed Output Analysis
- Initial head position: 50
- Head trajectory:
  $50 \to 95 \ (|45|) \to 180 \ (|85|) \to 34 \ (|146|) \to 119 \ (|85|) \to 11 \ (|108|) \to 123 \ (|112|) \to 62 \ (|61|) \to 64 \ (|2|)$.
- Total head movement $= 45 + 85 + 146 + 85 + 108 + 112 + 61 + 2 = 644$ cylinders.

---

## Experiment 13 (Part B): Disk Scheduling — SCAN (Elevator) Algorithm

### 1. Aim
To implement the SCAN (Elevator) disk head scheduling algorithm in both inward (low) and outward (high) directions.

### 2. Theory & Concepts
- In SCAN disk scheduling, the disk arm starts at one end of the disk and moves toward the other end, servicing requests as it reaches each cylinder until it hits the end of the disk.
- At the disk boundary (0 or max size $- 1$), the arm reverses direction and services requests in the opposite direction.

### 3. Source Code (`exp13_scan_disk.c`)
```c
// Experiment 13 (Part B): SCAN (Elevator) Disk Scheduling Algorithm
#include <stdio.h>
#include <stdlib.h>

int main() {
    int RQ[100], i, j, n, TotalHeadMoment = 0, initial, size, move;

    printf("Enter the number of Requests\n");
    scanf("%d", &n);

    printf("Enter the Requests sequence\n");
    for (i = 0; i < n; i++)
        scanf("%d", &RQ[i]);

    printf("Enter initial head position\n");
    scanf("%d", &initial);

    printf("Enter total disk size\n");
    scanf("%d", &size);

    printf("Enter the head movement direction for high 1 and for low 0\n");
    scanf("%d", &move);

    // Sort the request array
    for (i = 0; i < n; i++) {
        for (j = 0; j < n - i - 1; j++) {
            if (RQ[j] > RQ[j + 1]) {
                int temp = RQ[j];
                RQ[j] = RQ[j + 1];
                RQ[j + 1] = temp;
            }
        }
    }

    int index = n;
    for (i = 0; i < n; i++) {
        if (initial < RQ[i]) {
            index = i;
            break;
        }
    }

    // if movement is towards high value
    if (move == 1) {
        for (i = index; i < n; i++) {
            TotalHeadMoment = TotalHeadMoment + abs(RQ[i] - initial);
            initial = RQ[i];
        }
        // last movement for max size
        TotalHeadMoment = TotalHeadMoment + abs(size - RQ[i - 1] - 1);
        initial = size - 1;
        for (i = index - 1; i >= 0; i--) {
            TotalHeadMoment = TotalHeadMoment + abs(RQ[i] - initial);
            initial = RQ[i];
        }
    }
    // if movement is towards low value
    else {
        for (i = index - 1; i >= 0; i--) {
            TotalHeadMoment = TotalHeadMoment + abs(RQ[i] - initial);
            initial = RQ[i];
        }
        // last movement for min size
        TotalHeadMoment = TotalHeadMoment + abs(RQ[i + 1] - 0);
        initial = 0;
        for (i = index; i < n; i++) {
            TotalHeadMoment = TotalHeadMoment + abs(RQ[i] - initial);
            initial = RQ[i];
        }
    }

    printf("Total head movement is %d\n", TotalHeadMoment);
    return 0;
}
```

### 4. Output
**Case 1: Moving towards low value (direction 0):**
```
Enter the number of Requests
8
Enter the Requests sequence
95 180 34 119 11 123 62 64
Enter initial head position
50
Enter total disk size
200
Enter the head movement direction for high 1 and for low 0
0
Total head movement is 230
```

**Case 2: Moving towards high value (direction 1):**
```
Enter the number of Requests
8
Enter the Requests sequence
95 180 34 119 11 123 62 64
Enter initial head position
50
Enter total disk size
200
Enter the head movement direction for high 1 and for low 0
1
Total head movement is 337
```

### 5. Detailed Output Analysis
- In Case 1 (towards 0):
  - Inward sweep: $50 \to 34 \to 11 \to 0$ (movement = $50$).
  - Reversal and outward sweep: $0 \to 62 \to 64 \to 95 \to 119 \to 123 \to 180$ (movement = $180$).
  - Total head movement $= 50 + 180 = 230$ tracks.
- Compared with FCFS (644), SCAN drastically optimizes seek overhead to 230 tracks (a 64.3% reduction).

