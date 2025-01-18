#include "workthread.h"

void WorkThread::run(){
    while (true){
        std::unique_lock<std::mutex> lock(_mutex);
        _condition.wait(lock, [this](){
            return _stop || !_taskQueue.empty();
        });
        if(_stop && _taskQueue.empty()) return;
        auto task = std::move(_taskQueue.front());
        _taskQueue.pop();

        try {
            task();
        }catch (std::exception& ex){
            std::cout << ex.what() << std::endl;
        }
    }
};

WorkThread::WorkThread():_stop(false){
    _thread = std::make_shared<std::thread>(&WorkThread::run, this);
};


WorkThread::~WorkThread(){
    _stop = true;
    _condition.notify_one();
    _thread->join();
};
