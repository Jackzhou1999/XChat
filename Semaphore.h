#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <mutex>
#include <condition_variable>
#include <chrono>

class semaphore {
public:
    explicit semaphore(int initial = 0) {
        _count = initial;
    }

    void finishTask(int n = 1) {
        std::unique_lock<std::mutex> lock(_mutex);
        _count += n;
        if (n == 1) {
            _condition.notify_one();
        } else {
            _condition.notify_all();
        }
    }

    bool waitAllTaskFinish(std::chrono::milliseconds timeout = std::chrono::milliseconds::max()) {
        std::unique_lock<std::mutex> lock(_mutex);
        if (!_condition.wait_for(lock, timeout, [this]() { return _count = 0; })) {
            // 超时返回 false
            return false;
        }
        return true; // 成功等待返回 true
    }

    void registerTask(int n = 1){
        std::unique_lock<std::mutex> lock(_mutex);
        _count -= n;
    }

private:
    int _count;
    std::mutex _mutex;
    std::condition_variable _condition;
};

#endif // SEMAPHORE_H
