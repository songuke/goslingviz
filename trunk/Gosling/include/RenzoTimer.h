#ifndef TIMER_H_
#define TIMER_H_

#include "GosMustHave.h"

#ifdef WIN32
	#include "Windows.h"
#else
	#include <sys/time.h>
#endif

namespace Renzo 
{
class RENZODERER_API Timer
{
public:
	Timer();
	virtual ~Timer();
	
private:
	
	
	//int isRunning;
	int frames;
	int curFps;
	float avgFps;
	
	int loop;
	
	int stable;
	
	int milliseconds;
	long int timeElapsed;	
	long int totalTimeElapsed;
	
	#ifdef WIN32
		unsigned long       tv;
		unsigned long     oldTv;
	#else
		struct timeval tv;
		struct timeval oldTv;
	#endif
public:
	//void Start();
	//void Stop();
	void Reset();
	int Update(); //Auto update with internal delta-time
	void Update(int deltaTime); //Update with custom delta-time passed in
	void sleep(int deltaTime);

	float GetFps();	
	long int GetTimeElapsed();
	const char* ToString();
	float GetAverageFps();
};
}
#endif /*TIMER_H_*/
