#include <atomic>


class SpinLock {
 public:
  void lock() {
    while(a_lock.test_and_set(std::memory_order_acquire)) {
      //waiting here
    }
  }

  void unlock() {
    a_lock.clear(std::memory_order_release);
  }

 private:
  std::atomic_flag a_lock = ATOMIC_FLAG_INIT;

};
