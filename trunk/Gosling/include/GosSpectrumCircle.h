#ifndef _GOS_SPECTRUM_CIRCLE_
#define _GOS_SPECTRUM_CIRCLE_

#include "GosMustHave.h"
#include "GosVisualizer.h"

namespace Gos
{
class SpectrumCircle : public Visualizer
{
public:
	SpectrumCircle(void);
	~SpectrumCircle(void);

public:
	void render(Chunk& c, Rect r);

protected:
	//Chunk chunk1;
	//Chunk chunk2;
};
}

#endif
