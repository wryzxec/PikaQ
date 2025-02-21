#ifndef pika_queue_hpp
#define pika_queue_hpp
#include <atomic>
#include <new>

template<typename T, size_t capacity>
class Pika_Q {
public:

  /*
  *   Extra capacity slot allows for distinction between empty
  *   and full states for the queue.
  */

  Pika_Q()
    : m_head(0), m_tail(0),
      m_head_cached(0),
      m_tail_cached(0) {}
    
  Pika_Q& operator=(const Pika_Q&) = delete;
  Pika_Q(const Pika_Q&) = delete;

  ~Pika_Q() = default;

  bool push(const T& item) {
    size_t tail = m_tail.load(std::memory_order_relaxed);
    size_t next_tail = tail + 1;
    
    /*
    *   Modulo operator can be costly.
    */

    if(next_tail == m_capacity) {
      next_tail = 0;
    }

    if(next_tail == m_head_cached) {
      m_head_cached = m_head.load(std::memory_order_acquire);
      if(next_tail == m_head_cached) {
        return false;
      }
    }

    m_buffer[tail] = item;
    m_tail.store(next_tail, std::memory_order_release);
    return true;
  }

  bool pop(T& item) {
    size_t head = m_head.load(std::memory_order_relaxed);

    if(head == m_tail_cached) {
      m_tail_cached = m_tail.load(std::memory_order_acquire);
      if(head == m_tail_cached) {
        return false;
      }
    }

    item = m_buffer[head];
    size_t next_head = head + 1;
    if(next_head == m_capacity) {
      next_head = 0;
    }
    m_head.store(next_head, std::memory_order_release);
    return true;
  }

  bool empty() const {
    return m_tail.load(std::memory_order_acquire) == m_head.load(std::memory_order_acquire);
  }

  bool full() const {
    return ((m_tail.load(std::memory_order_acquire) + 1) % m_capacity) == m_head.load(std::memory_order_acquire);
  }

  size_t size() const {
    size_t head = m_head.load(std::memory_order_acquire);
    size_t tail = m_tail.load(std::memory_order_acquire);
    if(tail >= head) {
      return tail - head;
    }
    return m_capacity - head + tail;
  } 

private:

  /*
  *   Attempts to get size of machine's cache lines
  *   aligning head and tail pointers on separate cache lines
  *   to prevent false sharing.
  */

  #ifdef __cpp_lib_hardware_interference_size
    static constexpr size_t cache_line_size = std::hardware_destructive_interference_size;
  #else
    static constexpr size_t cache_line_size = 64; // default
  #endif

  static constexpr size_t m_capacity = capacity + 1;
  alignas(cache_line_size) T m_buffer[m_capacity];
    
  alignas(cache_line_size) std::atomic<size_t> m_head;
  alignas(cache_line_size) std::atomic<size_t> m_tail;

  alignas(cache_line_size) size_t m_head_cached;
  alignas(cache_line_size) size_t m_tail_cached;
};

#endif
