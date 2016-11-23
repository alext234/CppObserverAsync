#include <iostream>
#include <string>
#include <memory>
#include <thread>
#include <chrono>
#include "CppObserverAsync.h"

using namespace std::chrono_literals;

struct EventMsg {
    EventMsg (std::string  message): msg{message} {}
    std::string msg;
};

class A: public ObservableAsync<EventMsg>{
public:
    void doSomeWorkAndNotifyAsync () {
        notifyObservers (EventMsg{"hello from A!"});        
    }

    void doSomeWorkAndNotifyNoAsync () {
        notifyObserversNoAsync (EventMsg{"hello from A!"});        
    }
};

class B: public AbstractObserverAsync<EventMsg>{
public:    

    void onNotified (const EventMsg& msg) override {
        std::cout <<"B: received message : "<< msg.msg <<std::endl;
        std::cout <<"B: start long processing operation.."<<std::endl;
        std::this_thread::sleep_for (3s);
        
    }

    
};




int main (int argc, char* argv[]) {
    auto a = std::make_unique<A> ();

    auto b = std::make_shared<B> ();
    
    a->registerObserver(b);
    a->registerObserver([](const EventMsg& msg) {
        std::cout <<"lambda: received message : "<< msg.msg <<std::endl;
        std::cout <<"lambda: start long processing operation.."<<std::endl;
        std::this_thread::sleep_for (3s);
    });
   
    std::cout<<"================================"<<std::endl;
    std::cout<<"A notify observers one by one:"<<std::endl;
    auto t1 = std::chrono::high_resolution_clock::now();
    a->doSomeWorkAndNotifyNoAsync(); // notify one by one
    auto t2 = std::chrono::high_resolution_clock::now();
    unsigned long d = std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count();    
    std::cout<<"duration (ms):"<<d<<std::endl;


    std::cout<<"================================"<<std::endl;
    std::cout<<"A notify observers asynchronously:"<<std::endl;
    t1 = std::chrono::high_resolution_clock::now();
    a->doSomeWorkAndNotifyAsync(); // after sending notify, wait for all observers to finish
    t2 = std::chrono::high_resolution_clock::now();
    d = std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count();    
    std::cout<<"duration (ms):"<<d<<std::endl;
    
}
