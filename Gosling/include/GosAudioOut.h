#ifndef _GOS_AUDIO_OUT_
#define _GOS_AUDIO_OUT_

#include "GosMustHave.h"

#include "RtWvOut.h"
using namespace stk;

namespace Gos {
class AudioOut
{
public:
	AudioOut(void);
	~AudioOut(void);

public:
	void setChannels(int channels);

	/**
	Send a sample to output device (default)
	*/
	void tick(double sample);
	
	/**
	Send a chunk of samples to output device
	*/
	void tickChunk(Chunk& c);

protected:
	int			channels;
	RtWvOut*	wvOut;
	StkFrames*	frames;
};
}

#endif