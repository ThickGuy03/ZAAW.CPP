#include <iostream>
#include <thread>
#include <mutex>


std::mutex coutMutex;

unsigned int generateThreadId() {
    // Symulacja generowania unikalnego identyfikatora wątku
    static std::atomic<unsigned int> nextThreadId = 1;
    return nextThreadId.fetch_add(1, std::memory_order_relaxed);
}

void printWithThreadId(const std::string& text) {
    std::lock_guard<std::mutex> lock(coutMutex);
    std::cout << "[" << generateThreadId() << "] " << text << std::endl;
}

int main() {
    printWithThreadId("Hello from main thread!");

    std::thread thread1([]() {
        printWithThreadId("Hello from additional thread!");
        });

    thread1.join();

    return 0;
}
