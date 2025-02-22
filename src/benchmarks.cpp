#include <benchmark/benchmark.h>
#include "../include/pika_queue.hpp"
#include <boost/lockfree/spsc_queue.hpp>
#include <folly/ProducerConsumerQueue.h>
#include <thread>

constexpr int QUEUE_SIZE = 1000000;
constexpr int NUM_ITERS = 50000;

template <typename QueueType, typename PushFunc, typename PopFunc>
static void spsc_queue_benchmark(benchmark::State& state, QueueType& queue, PushFunc push, PopFunc pop) {
    for (auto _ : state) {
        std::thread prod([&queue, push] {
            for (int i = 0; i < NUM_ITERS; ++i) {
                while (!push(queue, i)) {
                }
                benchmark::DoNotOptimize(i);
            }
        });

        std::thread cons([&queue, pop] {
            int val{};
            for (int i = 0; i < NUM_ITERS; ++i) {
                while (!pop(queue, val)) {
                }
                benchmark::DoNotOptimize(val);
            }
        });

        prod.join();
        cons.join();

        state.SetItemsProcessed(NUM_ITERS);
        state.counters["Throughput"] = benchmark::Counter(NUM_ITERS, benchmark::Counter::kIsRate);
    }
}

Pika_Q<int, QUEUE_SIZE> custom_q;
static void pika_q_benchmark(benchmark::State& state) {
    spsc_queue_benchmark(state, custom_q, 
        [](auto& q, int val) { return q.push(val); },
        [](auto& q, int& val) { return q.pop(val); });
}
BENCHMARK(pika_q_benchmark)->Unit(benchmark::kMillisecond);

boost::lockfree::spsc_queue<int> boost_q(QUEUE_SIZE);
static void boost_q_benchmark(benchmark::State& state) {
    spsc_queue_benchmark(state, boost_q,
        [](auto& q, int val) { return q.push(val); },
        [](auto& q, int& val) { return q.pop(val); });
}
BENCHMARK(boost_q_benchmark)->Unit(benchmark::kMillisecond);

folly::ProducerConsumerQueue<int> folly_q(QUEUE_SIZE);
static void folly_q_benchmark(benchmark::State& state) {
    spsc_queue_benchmark(state, folly_q,
        [](auto& q, int val) { return q.write(val); },
        [](auto& q, int& val) { return q.read(val); });
}
BENCHMARK(folly_q_benchmark)->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();

