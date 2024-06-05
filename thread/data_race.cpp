#include <iostream>
#include <thread>

static unsigned int count=0;
void func1(){
    for(int i=0;i<1000000;i++){
        count++;
    }
}

void func2()
{
    for(int i=0;i<100000;i++){
        count++;
    }
}

int main(){
    std::thread t1(func1);
    std::thread t2(func2);

    t1.join();
    t2.join();

    printf("count=%d\n",count);
    return 0;
}