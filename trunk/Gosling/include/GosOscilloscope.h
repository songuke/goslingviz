#ifndef _GOS_OSCILLOSCOPE_
#define _GOS_OSCILLOSCOPE_

#include "GosMustHave.h"
#include "GosVisualizer.h"
#include "GosChunk.h"
#include <deque>

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
	void renderChunk(Chunk& c, Rect r, float alpha);
	std::deque<Chunk> q;
	static const int queueSize = 4;
	static float alpha[queueSize];
};
}

#endif
