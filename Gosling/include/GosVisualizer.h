#ifndef _GOS_VISUALIZER_
#define _GOS_VISUALIZER_

#include "GosMustHave.h"

namespace Gos 
{
class Visualizer
{
public:
	Visualizer(void);
	virtual ~Visualizer(void);

public:
	virtual void update(int delta);
	virtual void render(Chunk& c, Rect r) = 0;
	virtual void sleep() { 
		// sleep
		sleepMiliseconds(100);
	};

	virtual void onKey(int key) {}

protected:
	long timeElapsed;
};
}

#endif
