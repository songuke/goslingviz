#include "GosBeatDetector.h"

namespace Gos
{
BeatDetector::BeatDetector()
{
	historySize = (int) floor((float) 2 * kSampleRate / kChunkSize);
	beatLength = 12;
	beatLengthCount = 0;
	//cMultiplier = -0.0025714;
	cMultiplier = -0.0001;
	cTranspose = 1.5142857;
	//cTranspose = 3.0;
}

BeatDetector::~BeatDetector()
{
}

int BeatDetector::hasBeat(Chunk &c)
{
	float currentE = calcInstantEnergy(c);
	float threshold = calcThreshEnergy();
	history.push_front(currentE);
	if (history.size() > historySize)
	{
		history.pop_back();
		if (beatLengthCount > 0)
		{
			beatLengthCount--;
			return 1;
		}
		else if (currentE > threshold)
		{
			beatLengthCount = beatLength;
			//printf("%f - %f\n", currentE, threshold); //DEBUG
			return 2;
		}
	}
	return 0;
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
	//var = sqrt(var);

	//printf("var %f, avge %f\n", var, avgE); //DEBUG
	return (cMultiplier * var + cTranspose) * avgE;
	//return 1.3 * avgE;
}

}