#include "GosMustHave.h"

#ifndef _GOS_CHUNK_
#define _GOS_CHUNK_

namespace Gos
{
struct Chunk {
	double amplitude	[kChunkSize][nbChannels]; // amplitude in time domain
	double magnitude	[kChunkSize][nbChannels]; // magnitude of FFT transform
	double phase		[kChunkSize][nbChannels]; // phase of FFT transform
	int beat;									  // whether chunk contains a beat
};

}

#endif