#pragma once
#include <condition_variable>
#include <exception>
#include <functional>
#include <mutex>
#include <stdexcept>
#include <thread>

namespace r05 {
// Synchronous dispatch on one durable caller thread. Each submission completes
// or throws before returning; there is no background planning between calls.
class PersistentWorker {
public:
    PersistentWorker():thread_([this]{loop();}){}
    PersistentWorker(const PersistentWorker&)=delete;
    PersistentWorker& operator=(const PersistentWorker&)=delete;
    ~PersistentWorker() {
        {std::lock_guard<std::mutex> lock(mutex_);stopping_=true;}
        ready_.notify_one();thread_.join();
    }
    void run(std::function<void()> work) {
        if(std::this_thread::get_id()==thread_.get_id())throw std::logic_error("recursive worker dispatch");
        std::unique_lock<std::mutex> lock(mutex_);
        if(pending_ || stopping_)throw std::logic_error("concurrent or stopped worker dispatch");
        error_=nullptr;work_=std::move(work);pending_=true;ready_.notify_one();
        done_.wait(lock,[this]{return !pending_;});
        if(error_)std::rethrow_exception(error_);
    }
private:
    void loop() {
        std::unique_lock<std::mutex> lock(mutex_);
        for(;;) {
            ready_.wait(lock,[this]{return pending_ || stopping_;});
            if(!pending_ && stopping_)return;
            auto work=std::move(work_);lock.unlock();
            std::exception_ptr error;
            try{work();}catch(...){error=std::current_exception();}
            lock.lock();error_=error;pending_=false;done_.notify_one();
        }
    }
    std::mutex mutex_;
    std::condition_variable ready_,done_;
    std::function<void()> work_;
    std::exception_ptr error_;
    bool pending_=false,stopping_=false;
    std::thread thread_;
};
}
