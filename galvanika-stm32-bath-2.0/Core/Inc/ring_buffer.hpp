#ifndef INC_RING_BUFFER_HPP_
#define INC_RING_BUFFER_HPP_

#include <stdint.h>

template<typename Type, uint8_t capacity>
struct RingBuffer {
private:
  Type buffer[capacity] = { };
  volatile uint8_t head = 0;
  volatile uint8_t tail = 0;
  volatile uint8_t size = 0;

  inline
  void add_inner(const Type item) {
    buffer[tail++] = item;
    if (tail == capacity) {
      tail = 0;
    }
    size++;
  }

public:
  inline
  bool is_full() const {
    return size == capacity;
  }

  inline
  bool is_empty() const {
    return size == 0;
  }

  inline uint8_t get_size() const {
    return size;
  }

  bool offer(const Type item) {
    if (!is_full()) {
      add_inner(item);
      return true;
    }
    return false;
  }

  void add(const Type item) {
    if (is_full()) {
      poll();
    }
    add_inner(item);
  }

  Type peek() {
    return buffer[head];
  }

  Type poll() {
    Type item = buffer[head++];
    if (head == capacity) {
      head = 0;
    }
    size--;
    return item;
  }

  void clear() {
    head = 0;
    tail = 0;
    size = 0;
  }

  bool reset() {
    bool is_valid = false;
    if (size <= capacity && head < capacity && tail < capacity) {
      if (head > tail) {
        is_valid = head - tail != capacity - size;
      } else {
        is_valid = tail - head != size - 1;
      }
    }
    if (!is_valid) {
      clear();
    }
    return !is_valid;
  }

  template<typename Action>
  void iterate_reverse(Action &&on_item) const {
    if (size <= capacity) {
      uint8_t i = 0;
      while (i < size) {
        const uint8_t real_index = (tail > i ? 0 : capacity) + tail - i - 1;
        on_item(buffer[real_index], i);
        i++;
      }
    }
  }

  template<typename Action>
  void iterate(Action &&on_item) const {
    if (size <= capacity) {
      uint8_t i = 0;
      while (i < size) {
        uint8_t real_index = head + i;
        if (real_index >= capacity) {
          real_index -= capacity;
        }
        on_item(buffer[real_index], i);
        i++;
      }
    }
  }

  Type average() {
    if (size == 0) {
      return 0;
    }
    Type total = 0;
    iterate([&](const Type& item, uint8_t idx) -> void {
      total += item;
    });
    return total / size;
  }

};

#endif
