#ifndef _GOS_BEAT_DETECTOR_
#define _GOS_BEAT_DETECTOR_

#include "GosMustHave.h"
#include "GosChunk.h"
#include <deque>

namespace Gos
{
class BeatDetector
{
public:
	BeatDetector();
	~BeatDetector();
	bool hasBeat(Chunk& c);

private:
	float calcInstantEnergy(Chunk& c);
	float calcThreshEnergy();

private:
	float cMultiplier;
	float cTranspose;
	int historySize;
	int beatLength;
	int beatLengthCount;
	std::deque<float> history;
};

}

#endif