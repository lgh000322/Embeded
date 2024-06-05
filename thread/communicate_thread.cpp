#include <iostream>
#include <thread>
#include <mutex>
#include <queue>

std::mutex m;
std::queue<int> q;
unsigned int count=10;

void producer()
{
    for(int i=0;i<count;i++)
    {
        m.lock();
        q.push(i);
        m.unlock();
    }
}

void consumer(){
    int num_received=0;
    while(num_received<count){
        m.lock();
        if(q.empty()){
            m.unlock();
            continue;
        }

        int data=q.front();
        q.pop();
        m.unlock();
        printf("Data %d received\n",data);
        num_received++;
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
