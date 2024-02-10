/**
 * This will provide two Implentations
 * 1) Normal Spin Lock - based on atomic_flag - Consumes CPU Resources wihle waiting for aquiring lock
 * 2) More efficient Spin Lock - based on conditional Variables - Does not consume CPU Resources wihle waiting for aquiring lock
 * 
*/

#include <atomic>
#include <thread>
#include <iostream>
#include <mutex>
#include <condition_variable>

/*
    atomic operations for the lock mechanism, suitable for high-performance, low-latency scenarios where the lock is held for a very short time
*/
static int counter = {0};
namespace atomic_sl
{
    class SpinLock{
    public:
        void lock()
        {
            // If some other thread is already aquired this lock, test_and_set will return True; so the while loop will be repeated;
            // If no one has the lock; test_and_set would return False - While loop ends,and Atomic operation will set it to True - indicating we got the lock
            while (m_lock.test_and_set()){} 
        }
        void unlock()
        {
            m_lock.clear();
        }
    private:
        std::atomic_flag m_lock = ATOMIC_FLAG_INIT;
    };
}

namespace condvar_sl
{
    /***
     * more appropriate for situations where the lock may be held for longer durations or 
     * when you want to avoid busy-waiting in scenarios with less contention but potentially longer wait times.
    */
    class SpinLock
    {
    public:
        void lock()
        {
            std::unique_lock<std::mutex> lock(m_mtx);
            // wait causes the current thread to block until 1) the condition variable is notified or 2) a spurious wakeup occurs
            // To make sure we don't stop waiting because of Spurious Wakeup, we pass on a lambda that will do a re-check
            m_cv.wait(lock,[this](){return !this->m_is_locked;});
            m_is_locked = true;
        }

        void unlock()
        {
            {
                std::lock_guard<std::mutex> guard(m_mtx);
                m_is_locked = false;
            }
            m_cv.notify_one(); // Wake one waiting thread, if any.
        }
    private:
        std::mutex m_mtx;
        bool m_is_locked = {false};
        std::condition_variable m_cv;
    };
}

template <typename Lock>
void Work_on_resource(Lock& mylock)
{
    std::cout << "1) Given a long enough String " << " and more number of threads" << " This Part will be messed up" << " Since it is not protected\n";
    std::cout << "2) Given a long enough String " << " and more number of threads" << " This Part will be messed up" << " Since it is not protected\n";
    std::cout << "3) Given a long enough String " << " and more number of threads" << " This Part will be messed up" << " Since it is not protected\n";
    std::cout << "4) Given a long enough String " << " and more number of threads" << " This Part will be messed up" << " Since it is not protected\n";
    mylock.lock();
    counter++;
    std::cout << "(Thread = " << counter << ")\n";
    std::cout << "\t1)"  << "-------------------------This is a Protected section " << " This Section will not be messed up ---------------------------------\n";
    std::cout << "\t2)"  << "-------------------------This is a Protected section " << " This Section will not be messed up ---------------------------------\n";
    std::cout << "\t3)"  << "-------------------------This is a Protected section " << " This Section will not be messed up ---------------------------------\n";
    std::cout << "\t4)"  << "-------------------------This is a Protected section " << " This Section will not be messed up ---------------------------------\n";
    mylock.unlock();
}

void TestSpinLock_atomic_flag_impl()
{
    atomic_sl::SpinLock my_lock1;
    std::thread t1(Work_on_resource<atomic_sl::SpinLock>, std::ref(my_lock1)); // Note: Passing lock by reference
    std::thread t2(Work_on_resource<atomic_sl::SpinLock>, std::ref(my_lock1));
    std::thread t3(Work_on_resource<atomic_sl::SpinLock>, std::ref(my_lock1));
    std::thread t4(Work_on_resource<atomic_sl::SpinLock>, std::ref(my_lock1));


    t1.join();
    t2.join();
    t3.join();
    t4.join();
}

void TestSpinLock_cond_var_impl()
{
    condvar_sl::SpinLock my_lock1;
    std::thread t1(Work_on_resource<condvar_sl::SpinLock>, std::ref(my_lock1)); // Note: Passing lock by reference
    std::thread t2(Work_on_resource<condvar_sl::SpinLock>, std::ref(my_lock1));
    std::thread t3(Work_on_resource<condvar_sl::SpinLock>, std::ref(my_lock1));
    std::thread t4(Work_on_resource<condvar_sl::SpinLock>, std::ref(my_lock1));


    t1.join();
    t2.join();
    t3.join();
    t4.join();
}

int main()
{

    std::cout << "####################### TEST Atomic Flag Based Spin Lock #######################\n";
    TestSpinLock_atomic_flag_impl();
    std::cout << "####################### TEST Conditional Variable Based Spin Lock #######################\n";
    TestSpinLock_cond_var_impl();
}