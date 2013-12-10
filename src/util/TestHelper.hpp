#include <mutex>
#include <condition_variable>
#include <chrono>

using namespace std;


class TimeoutException : public exception{
	virtual const char * what() const throw(){
		return "A test timeout observed, this indicates a test failure!";
	}
};

class ProtectRaceConditions{
public:
	void waitUntilSthHappened() throw(TimeoutException) {
        unique_lock<mutex> lk(m);
        if (sth_happened){
        	sth_happened = false;
        	return;
        }
        if ( cv.wait_for(lk, std::chrono::milliseconds(1000)) == cv_status::timeout){
        		throw TimeoutException();
        }
        sth_happened = false;
	}

	bool hasSthHappened(){
		return sth_happened;
	}

protected:

	bool sth_happened = false;

	mutex m;
	condition_variable cv;

	void sthHappens(){
		unique_lock<mutex> lk(m);
		sth_happened = true;
		cv.notify_one();
	}
};
