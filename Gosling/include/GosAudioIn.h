#ifndef _GOS_AUDIO_IN_
#define _GOS_AUDIO_IN_

#include "GosMustHave.h"
#include "GosBeatDetector.h"

#include "FileWvIn.h"
using namespace stk;

namespace Gos {
class AudioIn
{
public:
	AudioIn(void);
	~AudioIn(void);

public:
	void loadAudio(const String& file);

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
protected:
	FileWvIn* wvIn;
	unsigned long next;
	StkFrames* frames;

	fftw_complex *in, *out;
	fftw_plan plan;

	BeatDetector beatDetector;
};
}

#endif
