#include "counting_semaphore.h"

counting_semaphore::counting_semaphore(size_t _max):max_(_max), count_(0){}
 
void counting_semaphore::release() {
    --count_;
    condition_.notify_one();
}

void counting_semaphore::acquire() {
    std::unique_lock<decltype(mtx_)> lock(mtx_);
    while(count_ < max_)
        condition_.wait(lock);
    ++count_;
}

bool counting_semaphore::try_acquire() {
    std::lock_guard<decltype(mtx_)> lock(mtx_);
    if(count_) {
        ++count_;
        return true;
    }
    return false;
}

size_t counting_semaphore::get_max() const{
    return max_;
}

void counting_semaphore::set_max(size_t max) {
    max_ = max;
    condition_.notify_all();
}

counting_semaphore_guard::counting_semaphore_guard(counting_semaphore & _sem) : sem_(_sem){_sem.acquire();}
    
counting_semaphore_guard::~counting_semaphore_guard(){sem_.release();}
