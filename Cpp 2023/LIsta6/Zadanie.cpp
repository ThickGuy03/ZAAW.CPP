#include <iostream>
#include <functional>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

class ThreadPool {
public:
    ThreadPool(size_t numThreads) : stopFlag(false) {
        for (size_t i = 0; i < numThreads; ++i) {
            threads.emplace_back([this] {
                while (true) {
                    std::function<double()> task;

                    {
                        std::unique_lock<std::mutex> lock(queueMutex);
                        condition.wait(lock, [this] { return stopFlag || !tasks.empty(); });

                        if (stopFlag && tasks.empty()) {
                            return;
                        }

                        task = std::move(tasks.front());
                        tasks.pop();
                    }

                    double result = task();
                    {
                        std::lock_guard<std::mutex> lock(resultMutex);
                        results.push_back(result);
                    }
                }
             });
        }
    }

    ~ThreadPool() {
        stop();
        for (auto& thread : threads) {
            thread.join();
        }
    }

    void add_task(std::function<double()> task) {
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            tasks.push(std::move(task));
        }
        condition.notify_one();
    }

    double average() {
        std::lock_guard<std::mutex> lock(resultMutex);

        if (results.empty()) {
            return 0.0;
        }

        double sum = 0.0;
        for (double result : results) {
            sum += result;
        }

        return sum / results.size();
    }

    void stop() {
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            stopFlag = true;
        }
        condition.notify_all();
    }

private:
    std::vector<std::thread> threads;
    std::queue<std::function<double()>> tasks;
    std::vector<double> results;

    std::mutex queueMutex;
    std::mutex resultMutex;
    std::condition_variable condition;
    std::atomic<bool> stopFlag;
};

int main() {
    ThreadPool threadPool(4);

    threadPool.add_task([] { return 1.0; });
    threadPool.add_task([] { return 2.0; });
    threadPool.add_task([] { return 3.0; });

    threadPool.stop();

    double avg = threadPool.average();
    std::cout << "Average: " << avg << std::endl;

    return 0;
}
