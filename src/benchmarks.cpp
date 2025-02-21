#include <benchmark/benchmark.h>
#include "../include/pika_queue.hpp"
#include <boost/lockfree/spsc_queue.hpp>
#include <thread>

constexpr int QUEUE_SIZE = 1000000;
constexpr int NUM_ITERS = 10'000;

boost::lockfree::spsc_queue<int> boost_q(QUEUE_SIZE);
Pika_Q<int> custom_q(QUEUE_SIZE);

static void custom_q_benchmark(benchmark::State& state) {
  for (auto _ : state) {
    std::thread prod([] {
      for (int i = 0; i < NUM_ITERS; ++i) {
        while (!custom_q.push(i));
        benchmark::DoNotOptimize(i);  
      }
    });

    std::thread cons([] {
      int val{};
      for (int i = 0; i < NUM_ITERS; ++i) {
        while (!custom_q.pop(val));
        benchmark::DoNotOptimize(val);
      }
    });

    prod.join();
    cons.join();

    state.SetItemsProcessed(NUM_ITERS);
  }
}
BENCHMARK(custom_q_benchmark)->Iterations(NUM_ITERS)->Unit(benchmark::kMillisecond);

static void boost_spsc_benchmark(benchmark::State& state) {
  for (auto _ : state) {
    std::thread prod([] {
      for (int i = 0; i < NUM_ITERS; ++i) {
        while (!boost_q.push(i));
        benchmark::DoNotOptimize(i);
      }
    });

    std::thread cons([] {
      int val{};
      for (int i = 0; i < NUM_ITERS; ++i) {
        while (!boost_q.pop(val));
        benchmark::DoNotOptimize(val);
      }
    });

    prod.join();
    cons.join();
    
    state.SetItemsProcessed(NUM_ITERS);
  }
}
BENCHMARK(boost_spsc_benchmark)->Iterations(NUM_ITERS)->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();
