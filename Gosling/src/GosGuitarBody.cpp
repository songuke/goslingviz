#include "GosGuitarBody.h"
#include "FileWvIn.h"
using namespace stk;

namespace Gos
{
GuitarBody::GuitarBody()
{
	nbFilterSamples = 1500;
	memset(x, 0, sizeof(double) * 3);
	memset(y, 0, sizeof(double) * 3);
	memset(a, 0, sizeof(double) * 3);
	memset(b, 0, sizeof(double) * 3);
}

GuitarBody::~GuitarBody(void)
{
}

void GuitarBody::loadBodyResonance(const String& file) {
	try
	{
		FileWvIn* wvIn = new FileWvIn(file);
		wvIn->setRate(wvIn->getFileRate() / 44100);

		int nbSamples = wvIn->getSize();
		// nbFilterSamples should not be greater than the total number of samples
		nbFilterSamples = min(nbSamples, nbFilterSamples);

		for (int i = 0; i < nbFilterSamples; ++i) {
			wvIn->tick();
			filter.push_back(wvIn->lastOut());
		}
	} catch (boost::exception& x) {
		FileReadError e;
		e << ErrnoInfo(errno) << FileNameInfo(file);
		throw e;
	}

	// flip filter for convolution
	for (int i = 0; i < nbFilterSamples / 2; ++i) {
		double t = filter[i];
		filter[i] = filter[nbFilterSamples - 1 - i];
		filter[nbFilterSamples - 1 - i] = t;
	}

	// load buffer with 0
	for (int i = 0; i < filter.size(); ++i) {
		buffer.push_back(0);
	}
}

void GuitarBody::loadBodyBiQuad(const String& file) {
	try {
		FILE* f = fopen(file.c_str(), "r");
		if (f) {
			fscanf(f, "%lf %lf %lf", &b[0], &b[1], &b[2]);
			fscanf(f, "%lf %lf %lf", &a[0], &a[1], &a[2]);
			fclose(f);
		}
	} catch (boost::exception& x) {
		FileReadError e;
		e << ErrnoInfo(errno) << FileNameInfo(file);
		throw e;
	}
}

void GuitarBody::tic(double inputSample) {
	// BiQuad filter
	x[2] = x[1]; x[1] = x[0]; x[0] = inputSample;
	y[2] = y[1]; y[1] = y[0]; y[0] = b[0] * x[0] + b[1] * x[1] + b[2] * x[2] - a[1] * y[1] - a[2] * y[2];

	curSample = y[0];

	/*
	// Very big filter using impulse response directly
	curSample = 0;
	for (int i = 0; i < filter.size(); ++i) 
		curSample += buffer[i] * filter[i];
	buffer.erase(buffer.begin());
	buffer.push_back(inputSample);
	*/
}

double GuitarBody::sample() {
	return curSample;
}

}