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

protected:
	//Chunk chunk1;
	//Chunk chunk2;
};
}

#endif
