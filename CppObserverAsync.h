#ifndef __CPP_OBSERVER_ASYNC__
#define __CPP_OBSERVER_ASYNC__
#include <memory>
#include <vector>
#include <future>
#include <algorithm>

template <typename Event>
class AbstractObserverAsync {
public:
	virtual void onNotified (const Event& event) = 0;
	virtual ~AbstractObserverAsync() {}
};


template <typename Event>
class ObservableAsync{
using CallableFunc = std::function<void(const Event&)>;
public:
	
	void registerObserver (std::shared_ptr<AbstractObserverAsync<Event>> ob){
		_observers.push_back (ob);
	}
	void registerObserver (CallableFunc callable){
		struct CallableWrapper: public AbstractObserverAsync<Event> {
			CallableWrapper(CallableFunc func):_callable{func} {}
			void onNotified (const Event& event) override {
				_callable(event);	
			}
			CallableFunc _callable;
		};

		auto observer = std::make_shared<CallableWrapper>(callable);
		_observersFromLambda.push_back(observer);

		registerObserver(observer);
	}

	void deregisterObserver (std::shared_ptr<AbstractObserverAsync<Event>> ob){
		for (auto it = _observers.begin(); it !=_observers.end (); ) {
			
			auto obs = (*it).lock();
			if (obs) {
				if (obs.get()==ob.get()) {
					_observers.erase (it);
				} else ++it;
			} else ++it;
		}
	}
	void notifyObserversNoAsync(const Event& event){
		for (auto it = _observers.begin(); it !=_observers.end (); ++it) {
			auto obs = (*it).lock();
			if (obs) {
				obs->onNotified (event);
			}
			
		}

	}

	void notifyObservers(const Event& event) {
        notifyObserversAsync(event, true);
    }

	void notifyObserversAsync(const Event& event,bool waitAllCompleted=true){
        
        //http://stackoverflow.com/questions/30810305/confusion-about-threads-launched-by-stdasync-with-stdlaunchasync-parameter
        // use a vector to hold the futures; if not it will not run asynchronously 

        std::vector<std::future<void>> futureList; 

		for (auto it = _observers.begin(); it !=_observers.end (); ++it) {
			auto obs = (*it).lock();
			if (obs) {
                futureList.push_back(std::move(
                    
                    std::async(std::launch::async, [&event,obs]{
                        obs->onNotified (event);

                    }) ));

			}
			
		}
        if (waitAllCompleted) {
            // wait for the async to finish
            for_each(futureList.begin(), futureList.end(), [](std::future<void>& f){f.wait(); });
        }

	}

protected:
	std::vector<std::weak_ptr<AbstractObserverAsync<Event>> >  _observers;
	std::vector<std::shared_ptr<AbstractObserverAsync<Event>> >  _observersFromLambda; // as a place holder to keep these objects alive (i.e. from killing away by smartpointer)
};

#endif //__CPP_OBSERVER_ASYNC__
