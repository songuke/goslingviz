#ifndef _GOS_AUDIO_IN_
#define _GOS_AUDIO_IN_

#include "GosMustHave.h"
#include "GosBeatDetector.h"
#include "GosChunk.h"

#include "FileWvIn.h"
using namespace stk;

#include <vector>
using namespace std;

namespace Gos {
class AudioIn : FileChangedHandler
{
public:
	AudioIn(void);
	~AudioIn(void);

public:
	void loadAudio(const String& file);
	void observeBeatFor(Visualizer* vis);
	void removeObserveBeatFor(Visualizer* vis);

private:
	static float hamming(int n, int bigN);

public:
	/**
	Return one sample at a time
	*/
	void	tick();
	double	sample();

	/**
	Return a chunk of samples at a time
	*/
	//void	tickChunk();
	void	sampleChunk(Chunk& c);

	bool	hasNext() const;

	int		getChannels() const;

	void	onFileChanged(const String& file);
protected:
	FileWvIn* wvIn;
	unsigned long next;
	StkFrames* frames;

	Chunk* lastChunk;

	fftw_complex *in, *out;
	fftw_plan plan;

	BeatDetector beatDetector;				
	typedef std::vector<Visualizer*> VisualizerList;
	VisualizerList	observerVis;	// the visualizers to notify when a beat is detected.
};
}

#endif
