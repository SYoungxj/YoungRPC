#ifndef __LockQueue_H__
#define __LockQueue_H__

#include "NonCopyable.h"
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

using std::queue;
using std::thread;
using std::mutex;
using std::condition_variable;

template <class T>
class LockQueue : public NonCopyable
{
public:
    LockQueue()
    {

    }

    ~LockQueue()
    {

    }
    
    void Push(const T & data)
    {
        std::lock_guard<mutex> lg(mutex_);
        que_.push(data);
        notEmpty_.notify_one(); // 只有一个线程在取日志
    }

    T Pop()
    {
        std::unique_lock<mutex> ul(mutex_);
        while(que_.empty())
        {
            notEmpty_.wait(ul); 
        }

        T data = que_.front();
        que_.pop();
        return data;
    }

private:
    queue<T> que_;
    mutex mutex_;
    condition_variable notEmpty_;
};

#endif
