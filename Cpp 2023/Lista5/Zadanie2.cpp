#include <iostream>
#include <future>
#include <thread>
#include <sstream>
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

void asyncFunction(std::launch policy, int level) {
    std::stringstream ss;
    ss << "Async  level " << level << "  policy: " << ((policy == std::launch::async) ? "std::launch::async" : "std::launch::deferred");

    printWithThreadId(ss.str());

    if (level > 0) {
        auto future = std::async(policy, asyncFunction, policy, level - 1);
        future.get();
    }

    ss.str("");
    ss << "Async function level " << level << " completed";
    printWithThreadId(ss.str());
}

int main() {
    printWithThreadId("Hello from main thread!");

    const int nestingLevel = 3;

    printWithThreadId("Calling async function with std::launch::async");
    std::async(std::launch::async, asyncFunction, std::launch::async, nestingLevel).get();

    printWithThreadId("Calling async function with std::launch::deferred");
    std::async(std::launch::deferred, asyncFunction, std::launch::deferred, nestingLevel).get();

    return 0;
}
