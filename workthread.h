#ifndef WORKTHREAD_H
#define WORKTHREAD_H

#include <thread>
#include <memory>
#include <queue>
#include <functional>
#include <condition_variable>
#include <exception>
#include <iostream>
#include <future>

class WorkThread {
    using Task = std::function<void(void)>;
public:
    WorkThread();
    ~WorkThread();
    template <class F, class... Args>
    auto PostTaskToQueue(F&& f, Args&&... args)
        -> std::future<typename std::result_of<F(Args...)>::type> {
        using ReturnType = typename std::result_of<F(Args...)>::type;

        auto task = std::make_shared<std::packaged_task<ReturnType()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
            );

        std::future<ReturnType> result = task->get_future();
        {
            std::unique_lock<std::mutex> lock(_mutex);
            if (_stop)
                throw std::runtime_error("ThreadPool is stopped, cannot enqueue task");
            _taskQueue.emplace([task]() { (*task)(); });
        }
        _condition.notify_one();
        return result;
    }

private:
    void run();

    std::shared_ptr<std::thread> _thread;
    std::queue<Task> _taskQueue;
    std::mutex _mutex;
    std::condition_variable _condition;
    std::atomic<bool> _stop;
};


#endif // WORKTHREAD_H
