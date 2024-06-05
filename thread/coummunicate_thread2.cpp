#include <iostream>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>

std::mutex m;
std::queue<int> q;
std::condition_variable cv;
unsigned int count=10;

void producer()
{
    for(int i=0;i<count;i++)
    {
        m.lock();
        q.push(i);
        m.unlock();

        cv.notify_one();
    }
}

void consumer(){
    int num_received=0;
    while(num_received<count){
        std::unique_lock<std::mutex> lk(m);
        cv.wait(lk,[&]{return !q.empty()||num_received>=count;});
        if(num_received>=count){
            lk.unlock();
            return;
        }

        int data=q.front();
        q.pop();
        printf("Data %d received.\n",data);
        num_received++;

        lk.unlock();
    }
}

    int main()
    {
        std::thread t1(producer);
        std::thread t2(consumer);

        t1.join();
        t2.join();

        return 0;
    }
