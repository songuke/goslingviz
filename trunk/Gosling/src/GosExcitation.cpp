#include "GosExcitation.h"

#include <FileWvIn.h>
using namespace stk;

namespace Gos
{
Excitation::Excitation(void)
: index(0)
{
}

Excitation::~Excitation(void)
{
}

void Excitation::loadExcitationSource(const String& file) {
	try
	{
		FileWvIn* wvIn = new FileWvIn(file);
		wvIn->setRate(wvIn->getFileRate() / 44100);

		int nbSamples = wvIn->getSize();
		for (int i = 0; i < nbSamples; ++i) {
			wvIn->tick();
			excitationSrc.push_back(wvIn->lastOut());
		}
	} catch (boost::exception& x) {
		FileReadError e;
		e << ErrnoInfo(errno) << FileNameInfo(file);
		throw e;
	}
}

void Excitation::tick() {
	index = (index + 1) % excitationSrc.size();
}

double Excitation::sample() {
	return excitationSrc[index];
}

}