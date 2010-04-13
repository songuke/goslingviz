#ifndef _GOS_SPECTROGRAM_
#define _GOS_SPECTROGRAM_

#include "GosMustHave.h"
#include "GosVisualizer.h"

namespace Gos
{
class Spectrogram : public Visualizer
{
public:
	Spectrogram(void);
	~Spectrogram(void);

public:
	void render(Chunk& c, Rect r);
	void onFileChanged(const String& file);
	void onBeat();

protected:	
	bool needRender;
	Chunk* lastChunk;
	void renderChunk(Chunk& c, Rect r);
	float* coolFactor, *coolVel, *coolAccel;	
	int nbBars;

	float logBase, logStep;
};
}

#endif
