#ifndef _GOS_GLOBAL_VISUALIZER_
#define _GOS_GLOBAL_VISUALIZER_

#include "GosMustHave.h"
#include "GosVisualizer.h"
#include "GosCurveWarp.h"
#include "GosOscilloscope.h"
#include "GosSpectrogram.h"

namespace Gos {
/**
Global visualizer class that shows all visualizer at the same time.
*/
class GlobalVisualizer : public Visualizer
{
public:
	GlobalVisualizer(void);
	~GlobalVisualizer(void);

public:
	void update(int delta);
	void render(Chunk& c, Rect r);
	void sleep(); 
	void onKey(int key);

protected:
	Oscilloscope osc;
	Spectrogram spec;
	CurveWarp vis2d;
};
}

#endif