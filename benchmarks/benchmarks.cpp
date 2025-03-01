#include <thread>
#include <chrono>
#include <iostream>
#include <vector>
#include <numeric>
#include <functional>

#include "../include/pika_queue.hpp"
#include <boost/lockfree/spsc_queue.hpp>
#include <folly/ProducerConsumerQueue.h>

static constexpr int QUEUE_SIZE = 10'000;
static constexpr int NUM_ITERS  = 100'000;
static constexpr int NUM_RUNS   = 1000;

double pika_q_benchmark() {
    Pika_Q<int> pika_q(QUEUE_SIZE);
    size_t successfulPushes = 0;
    size_t successfulPops = 0;

    auto start = std::chrono::steady_clock::now();

    std::thread prod([&]() {
        for (int i = 0; i < NUM_ITERS; ++i) {
            while (!pika_q.push(i)) {
                std::this_thread::yield();
            }
            ++successfulPushes;
        }
    });

    std::thread cons([&]() {
        int val = 0;
        for (int i = 0; i < NUM_ITERS; ++i) {
            while (!pika_q.pop(val)) {
                std::this_thread::yield();
            }
            ++successfulPops;
            if (val == -1) std::cout << "";
        }
    });

    prod.join();
    cons.join();

    auto end = std::chrono::steady_clock::now();
    return std::chrono::duration<double>(end - start).count();
}

double boost_q_benchmark() {
    boost::lockfree::spsc_queue<int> boost_q(QUEUE_SIZE);
    size_t successfulPushes = 0;
    size_t successfulPops = 0;

    auto start = std::chrono::steady_clock::now();

    std::thread prod([&]() {
        for (int i = 0; i < NUM_ITERS; ++i) {
            while (!boost_q.push(i)) {
                std::this_thread::yield();
            }
            ++successfulPushes;
        }
    });

    std::thread cons([&]() {
        int val = 0;
        for (int i = 0; i < NUM_ITERS; ++i) {
            while (!boost_q.pop(val)) {
                std::this_thread::yield();
            }
            ++successfulPops;
            if (val == -1) std::cout << "";
        }
    });

    prod.join();
    cons.join();

    auto end = std::chrono::steady_clock::now();
    return std::chrono::duration<double>(end - start).count();
}

double folly_q_benchmark() {
    folly::ProducerConsumerQueue<int> folly_q(QUEUE_SIZE);
    size_t successfulPushes = 0;
    size_t successfulPops = 0;

    auto start = std::chrono::steady_clock::now();

    std::thread prod([&]() {
        for (int i = 0; i < NUM_ITERS; ++i) {
            while (!folly_q.write(i)) {
                std::this_thread::yield();
            }
            ++successfulPushes;
        }
    });

    std::thread cons([&]() {
        int val = 0;
        for (int i = 0; i < NUM_ITERS; ++i) {
            while (!folly_q.read(val)) {
                std::this_thread::yield();
            }
            ++successfulPops;
            if (val == -1) std::cout << "";
        }
    });

    prod.join();
    cons.join();

    auto end = std::chrono::steady_clock::now();
    return std::chrono::duration<double>(end - start).count();
}

double run_average_benchmark(const std::function<double()>& benchmark_func) {
    std::vector<double> run_times;

    for (int i = 0; i < NUM_RUNS; ++i) {
        double run_time = benchmark_func();
        run_times.push_back(run_time);
    }

    double total_time = std::accumulate(run_times.begin(), run_times.end(), 0.0);
    double avg_time = total_time / static_cast<double>(NUM_RUNS);
    return avg_time;
}

int main() {
    double ops = 2.0 * static_cast<double>(NUM_ITERS);

    std::cout << "-- Pika_Q --\n";
    double avg_time_pika = run_average_benchmark(pika_q_benchmark) * 1000;
    double throughput_pika = ops / avg_time_pika;
    std::cout << "Average Throughput    : " << throughput_pika << " ops/ms\n\n";

    std::cout << "-- Boost_Q --\n";
    double avg_time_boost = run_average_benchmark(boost_q_benchmark) * 1000;
    double throughput_boost = ops / avg_time_boost;
    std::cout << "Average Throughput    : " << throughput_boost << " ops/ms\n\n";

    std::cout << "-- Folly_Q --\n";
    double avg_time_folly = run_average_benchmark(folly_q_benchmark) * 1000;
    double throughput_folly = ops / avg_time_folly;
    std::cout << "Average Throughput    : " << throughput_folly << " ops/ms\n\n";
}

