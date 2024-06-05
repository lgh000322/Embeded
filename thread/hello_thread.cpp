#include <iostream>
#include <thread>

void func1(){
    for(int i=0;i<100;i++){
        std::cout<<"hello fromm 1"<<std::endl;
    }
}

void func2()
{
    for(int i=0;i<100;i++){
        std::cout<<"hello from 2"<<std::endl;
    }
}

int main(){
    std::thread t1(func1);
    std::thread t2(func2);

    t1.join();
    t2.join();

    return 0;
}