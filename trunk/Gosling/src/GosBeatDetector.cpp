#include "GosBeatDetector.h"

namespace Gos
{
BeatDetector::BeatDetector()
{
	historySize = (int) floor((float) kSampleRate / kChunkSize);
	cMultiplier = -0.0025714;
	cTranspose = 1.5142857;
}

BeatDetector::~BeatDetector()
{
}

bool BeatDetector::hasBeat(Chunk &c)
{
	float currentE = calcInstantEnergy(c);
	float threshold = calcThreshEnergy();
	history.push_front(currentE);
	if (history.size() > historySize)
	{
		history.pop_back();

		if (currentE > threshold)
		{
			//printf("%f - %f\n", currentE, threshold); //DEBUG
			return true;
		}
	}
	return false;
}

float BeatDetector::calcInstantEnergy(Chunk &c)
{
	float result = 0;
	for (int i=0; i<nbChannels; i++)
	{
		for (int j=0; j<kChunkSize; j++)
		{
			result += pow(c.amplitude[j][i], 2);
		}
	}

	return result;
}

float BeatDetector::calcThreshEnergy()
{
	//calc average local energy
	float avgE = 0;
	for (int i=0; i<history.size(); i++)
	{
		avgE += history.at(i);
	}
	avgE /= history.size();

	//calc variance of energies
	float var = 0;
	for (int i=0; i<history.size(); i++)
	{
		var += pow((history.at(i) - avgE), 2);
	}
	var /= history.size();

	return (cMultiplier * var + cTranspose) * avgE;
}

}