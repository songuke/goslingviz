#ifndef _GOS_OSCILLOSCOPE_
#define _GOS_OSCILLOSCOPE_

#include "GosMustHave.h"
#include "GosVisualizer.h"

namespace Gos
{
class Oscilloscope : public Visualizer
{
public:
	Oscilloscope(void);
	~Oscilloscope(void);

public:
	void render(Chunk& c, Rect r);

protected:
	//Chunk chunk1;
	//Chunk chunk2;
};
}

#endif
