#include "RenzoTimer.h"

namespace Renzo
{
Timer::Timer() : 
	//isRunning(1), // a clock must be always running
	frames(0), 
	curFps(0), 
	avgFps(0), 
	loop(0),
	stable(0),
	milliseconds(0),
	timeElapsed(0),
	totalTimeElapsed(0)
{	
	Update();
}

Timer::~Timer()
{
}

/*
void Timer::Start() {
	isRunning = 1;
}

void Timer::Stop() {
	isRunning = 0;	
}
*/

void Timer::Reset() {
	timeElapsed = 0;
	totalTimeElapsed = 0;
	frames = 0;
	curFps = 0;
	avgFps = 0;
	loop = 0;
}

int Timer::Update() {
	/* calculate delta time */
	#ifdef WIN32
		tv = GetTickCount();
	#else
    	gettimeofday(&tv, 0);
    #endif   	
	
   
	if (stable == 1) {
		#ifdef WIN32
			milliseconds = tv - oldTv;
		#else
			milliseconds = (tv.tv_sec - oldTv.tv_sec)*1000 + (tv.tv_usec - oldTv.tv_usec) / 1000;
		#endif
		/* internal update to calculate FPS */
		Update(milliseconds);
	} else {
		stable = 1; /* first round, just change flag and update old time */
	}
	
	/* timer */
	oldTv = tv;
	return milliseconds;
}

void Timer::Update(int deltaTime) {
	/* calculate delta time */	
	//if (isRunning) {
		timeElapsed += deltaTime;
		totalTimeElapsed += deltaTime;
		frames++;
		//printf("Current time %d\n", timeElapsed);
		if (timeElapsed > 1000) {
			//timeElapsed -= 1000;
			
			//current fps
			//curFps = frames;
			curFps = frames * 1000.0f / timeElapsed;

			//printf("Current frame %d\n", curFps);
			frames = 0;			
			timeElapsed = 0;
			
			//average fps
			//avgFps = avgFps == 0.0f ? (curFps) : (avgFps*loop + curFps) / (loop + 1);
			avgFps = curFps;
			loop++;			
		}
	//}
}

float Timer::GetFps() {return curFps;}
float Timer::GetAverageFps() {return avgFps;}
long int Timer::GetTimeElapsed() {return totalTimeElapsed;}
const char* Timer::ToString() {
	/* return time elapsed */
	int milli = totalTimeElapsed % 1000;
	long int secs = totalTimeElapsed / 1000;
	
	int hours = secs / 3600;
	secs = secs % 3600;
	int minutes = secs / 60;
	int seconds = secs % 60;
	
	char* time = new char[100];
	//sprintf(time, "%dh %2dm %2ds %3dms", hours, minutes, seconds, milli);
	sprintf(time, "%d:%02d:%02d.%03d", hours, minutes, seconds, milli);
	return time;
}

void Timer::sleep(int deltaTime) {
	#ifdef WIN32
		Sleep(deltaTime); // WinBase.h
	#endif
}

}
