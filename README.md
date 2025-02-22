<div align="center">

# PikaQ
  
A lightning fast, single-producer single-consumer (SPSC) queue, written in C++.

View all the details of the project 
[**HERE**](https://wryzxec.github.io/lockfree_spsc.html)

</div>

# Usage
- `Pika_Q<T>(size_t capacity);`

  create a queue of items of type `T`, with capacity `capacity`

- `bool push(const T& item)`

  push an item of onto the queue, returns `true` if successful, `false` otherwise

- `bool pop(T& item)`

  pop the first item in the queue and stores it in `item`

- `bool full()`

  returns `true` if queue is full, `false` otherwise

- `bool empty()`

  returns `true` if queue is empty, `false` otherwise

- `size_t size()`

  returns the number of available items currently in the queue
