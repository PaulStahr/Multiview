#include <mutex>
#include <atomic>
#include <condition_variable>

class counting_semaphore {
private:
    std::mutex mtx_;
    std::condition_variable condition_;
    std::atomic<size_t> max_;
    std::atomic<size_t> count_;

public:
    counting_semaphore(size_t _max);
    counting_semaphore(const counting_semaphore&) = delete;

    void release();

    void acquire();

    size_t get_max() const;
    
    void set_max(size_t max);

    bool try_acquire();
};

class counting_semaphore_guard
{
private:
    counting_semaphore & sem_;
public:
    counting_semaphore_guard(counting_semaphore & _sem);
    counting_semaphore_guard(const counting_semaphore_guard&) = delete;
    ~counting_semaphore_guard();
};
