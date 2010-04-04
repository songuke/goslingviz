#ifndef _GOS_GUITAR_BODY_
#define _GOS_GUITAR_BODY_

#include "GosMustHave.h"
#include <vector>
using namespace std;

namespace Gos
{
/**
Guitar body resonance is modeled as a digital filter. The body filter process samples from PluckedString.
*/
class GuitarBody
{
public:
	GuitarBody();
	~GuitarBody(void);

public:
	/**
	Load body resonance wave file.
	*/
	void loadBodyResonance(const String& file);

	/**
	The least-damped mode is represented in parametric form of a BiQuad. The filter parameters are loaded by calling this function.
	*/
	void loadBodyBiQuad(const String& file);

public:
	/**
	Output the current filtered sample by the body filter.
	*/
	double sample();
	
	/**
	Take a sample from the plucked string and perform filtering.
	The body class caches a number of samples from the input plucked string. The size of the cache is larger than or equal to the size of the body filter.
	*/
	void tic(double inputSample);
protected:
	//PluckedString* string;
	typedef std::vector<double> FilterVector;
	FilterVector filter;
	typedef std::vector<double> BufferVector;
	BufferVector buffer;
	double curSample;
	int nbFilterSamples;

	double a[3], b[3];					// BiQuad coefficients (parametric form of the least-damped mode)
	double x[3], y[3];					// cached samples for filter implementation (x[0], y[0] are latest samples)
};
}

#endif