#ifndef SINGLETON_H
#define SINGLETON_H
#include "global.h"

template<typename T>
class Singleton{
public:
    static std::shared_ptr<T> GetInstance();
    ~Singleton(){
        std::cout<<"singleton instance delete"<<std::endl;
    }
protected:
    Singleton() = default;
    static std::shared_ptr<T> _instance;
private:
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

};

template<typename T>
std::shared_ptr<T> Singleton<T>::GetInstance(){
    static std::once_flag s_flag;
    std::call_once(s_flag, [&](){
        _instance = std::shared_ptr<T>(new T);
    });
    return _instance;
}

template<typename T>
std::shared_ptr<T> Singleton<T>::_instance = nullptr;

#endif // SINGLETON_H
