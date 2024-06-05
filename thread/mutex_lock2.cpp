#include <iostream>
#include <thread>
#include <mutex>

std::mutex m;
static unsigned int count=0;

void func1(){
    unsigned int my_count=0;
    for(int i=0;i<10000;i++)
    {
        my_count++;
    }
    m.lock();
    count+=my_count;
    m.unlock();
}

void func2(){
    unsigned int my_count=0;
    for(int i=0;i<10000;i++)
    {
        my_count++;
    }
    m.lock();
    count+=my_count;
    m.unlock();
}

int main(){
    std::thread t1(func1);
    std::thread t2(func2);

    t1.join();
    t2.join();

    printf("count= %d\n",count);
    return 0;
}