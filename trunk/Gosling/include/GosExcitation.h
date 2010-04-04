#ifndef _GOS_EXCITATION_
#define _GOS_EXCITATION_

#include "GosMustHave.h"
#include <vector>
using namespace std;

namespace Gos
{
class Excitation
{
public:
	Excitation(void);
	~Excitation(void);

public:
	/**
	Load precomputed excitation source. 
	*/
	void loadExcitationSource(const String& file);

	/**
	Return current sample of the excitation
	*/
	double sample();

	/**
	Advance to next sample
	*/
	void tick();
protected:
	typedef std::vector<double> ExcitationVector;
	ExcitationVector excitationSrc;							// excitations source
	
	int index;												// current sample index
};
}

#endif