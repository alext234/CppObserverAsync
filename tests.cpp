#include "gmock/gmock.h"
#include "CppObserverAsync.h"
#include <cstdlib>
#include <ctime>
#include <thread>

using namespace std;
using namespace testing;

struct TestEvent {
	TestEvent (int eventId) : _eventId{eventId} {}
	int _eventId;
};

class Subject: public ObservableAsync<TestEvent> {
public:
};


enum{OBS_PROCESSING_MILLISECONDS=200};

class Observer: public AbstractObserverAsync<TestEvent>{
public: 
	Observer() : _receivedEventId{0} {}
	void onNotified(const TestEvent& event) override {
		_receivedEventId = event._eventId;
        std::this_thread::sleep_for(std::chrono::milliseconds(OBS_PROCESSING_MILLISECONDS)); 
	}
	volatile int _receivedEventId;
};

TEST(CPP_OBSERVER, registerAndNotifyNoAsync ) {
	Subject subject;
    vector<std::shared_ptr<Observer>> observers;
    observers.push_back (make_shared<Observer>());
    observers.push_back (make_shared<Observer>());
    observers.push_back (make_shared<Observer>());
    observers.push_back (make_shared<Observer>());
    observers.push_back (make_shared<Observer>());


    for (auto ob: observers) {
        ob->_receivedEventId = 0;
	    subject.registerObserver (ob);
    }

	std::srand(std::time(0));
	const int TEST_EVENT_ID = std::rand();
	TestEvent event{TEST_EVENT_ID};

    auto t1 = std::chrono::high_resolution_clock::now();
	subject.notifyObserversNoAsync(event); // notify one by one

    auto t2 = std::chrono::high_resolution_clock::now();

    unsigned long d = std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count();

    EXPECT_THAT (d, Ge(OBS_PROCESSING_MILLISECONDS*observers.size()));
        
    for (auto ob: observers) {
	    EXPECT_THAT (ob->_receivedEventId, Eq(TEST_EVENT_ID));
    }
	
	
}

TEST(CPP_OBSERVER, registerAndNotifyAsyncNoWait ) {
	
	
	Subject subject;
    vector<std::shared_ptr<Observer>> observers;
    observers.push_back (make_shared<Observer>());
    observers.push_back (make_shared<Observer>());
    observers.push_back (make_shared<Observer>());
    observers.push_back (make_shared<Observer>());
    observers.push_back (make_shared<Observer>());


    for (auto const& ob: observers) {
        ob->_receivedEventId = 0;
	    subject.registerObserver (ob);
    }

	std::srand(std::time(0));
	const int TEST_EVENT_ID = std::rand();
	TestEvent event{TEST_EVENT_ID};

    auto t1 = std::chrono::high_resolution_clock::now();
	subject.notifyObserversAsync(event, false); 

    auto t2 = std::chrono::high_resolution_clock::now();

    unsigned long d = std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count();

    EXPECT_THAT (d, Lt(OBS_PROCESSING_MILLISECONDS*observers.size()));

    std::this_thread::sleep_for(std::chrono::milliseconds((observers.size()-1)*OBS_PROCESSING_MILLISECONDS)); 
        
    for (auto const &ob: observers) {
	    EXPECT_THAT (ob->_receivedEventId, Eq(TEST_EVENT_ID));
    }
}

TEST(CPP_OBSERVER, registerAndNotifyAsyncWait ) {
	
	
	Subject subject;
    vector<std::shared_ptr<Observer>> observers;
    observers.push_back (make_shared<Observer>());
    observers.push_back (make_shared<Observer>());
    observers.push_back (make_shared<Observer>());
    observers.push_back (make_shared<Observer>());
    observers.push_back (make_shared<Observer>());


    for (auto const& ob: observers) {
        ob->_receivedEventId = 0;
	    subject.registerObserver (ob);
    }

	std::srand(std::time(0));
	const int TEST_EVENT_ID = std::rand();
	TestEvent event{TEST_EVENT_ID};

	subject.notifyObserversAsync(event, true); 
        
    for (auto const &ob: observers) {
	    EXPECT_THAT (ob->_receivedEventId, Eq(TEST_EVENT_ID));
    }
}
int main(int argc, char *argv[])
{
	testing::InitGoogleMock (&argc, argv);
	return RUN_ALL_TESTS();
}
