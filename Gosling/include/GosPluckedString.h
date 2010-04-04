#ifndef _GOS_PLUCKED_STRING_
#define _GOS_PLUCKED_STRING_

#include "GosMustHave.h"
#include "GosRingBuffer.h"

namespace Gos
{
enum ExcitationType {
	WhiteNoise,
	SineWave,
	SquareWave
};

class PluckedStringParameter : Parameter
{
public:
	double amplitude;
	double gain;
	ExcitationType excitationType;
public:
	PluckedStringParameter() {
		// default parameters
		amplitude		= 0.5;
		gain			= 0.99;
		excitationType	= WhiteNoise;
	}
};

class PluckedString
{
public:
	PluckedString(double frequency);
	PluckedString(double* init, int n);

	virtual ~PluckedString(void);
public:
	/**
	Set the buffer to white noise, or precomputed excitation source if the source is available.
	*/
	void pluck();

	/**
	Set the buffer to precomputed excitation source
	*/
	void setExcitation(Excitation* ext);

	/**
	Return current sample (front of the ring buffer)
	*/
	double sample();

	/**
	Advance the simulation one step in time
	*/
	void tic();

	/**
	Return current time
	*/
	int timeElapsed();
	double frequency() const;

	/**
	Get the maximum value of a sample available
	*/
	double maxSample();

	/**
	Set adjustable parameters
	*/
	void setPluckedStringParameter(PluckedStringParameter* param);
	const PluckedStringParameter* getPluckedStringParameter() const;

protected:
	RingBuffer<double> buffer;					// internal ring buffer to store samples
	double freq;								// the frequency of this string
	int samplingRate;							// sampling rate, also the capacity of the ring buffer
	PluckedStringParameter	param;				// adjustable parameters
	Excitation*				excitationSrc;		// excitation source
	RingBuffer<double>		excitationCache;	// excitation cache (for speed up when plucking a string, we do not need to reload each sample from excitationSrc, but to copy from excitationCache)
};
}

#endif
