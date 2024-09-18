#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <thread>
#include <vector>
#include <queue>
#include <condition_variable>
#include <mutex>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <numeric>
#include "http_request.h"

template <class T>
class ThreadPool {
    public:
        ThreadPool(int);
        virtual ~ThreadPool();
        static void worker(void*);
        void shotdown();
        void append(T*);

    private:
        std::queue<T*> taskQueue;   //  工作队列
        int totalNum; // 总线程数量
        std::mutex mutexPool;   //  工作队列的锁
        std::condition_variable cond;   // 条件变量
        std::vector<std::thread> threads; // 线程池中的线程集合
        int close; // 线程池关闭标志，0为开启，1为关闭
    
};

template <typename T>
ThreadPool<T>::ThreadPool(int poolNum) {
    totalNum = poolNum;
    close = 0;

    threads.resize(totalNum);
    for (int i = 0; i < totalNum; i++) {
        threads[i] = std::thread(worker, this);
    }
}

template <typename T>
ThreadPool<T>::~ThreadPool() {
    //唤醒阻塞的工作线程
    cond.notify_all();
    for (int i = 0; i < totalNum; ++i) {
        if (threads[i].joinable()) {
            threads[i].join();
        }
    }
}

template <typename T>
void ThreadPool<T>::shotdown() {
    close = 1;
}

template <typename T>
void ThreadPool<T>::append(T* task) {
    std::unique_lock<std::mutex> lk(mutexPool);
    taskQueue.push(task);
    // 通知线程处理
    cond.notify_one();
    lk.unlock();
}

template <typename T>
void ThreadPool<T>::worker(void* arg) {
    ThreadPool* pool = static_cast<ThreadPool*>(arg);

    while(!pool->close) {
        std::unique_lock<std::mutex> lk(pool->mutexPool);

        while(pool->taskQueue.empty()) {
            if(pool->close)
                return;
            pool->cond.wait(lk);
        }

        T* task = pool->taskQueue.front();

        pool->taskQueue.pop();
        lk.unlock();

        task->process();
    }
}

#endif

