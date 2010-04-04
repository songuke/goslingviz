#include "GosMustHave.h"
#include "GosRingBuffer.h"
#include "GosPluckedString.h"
#include "GosExcitation.h"
#include <time.h>
#include <limits>

namespace Gos
{
PluckedString::PluckedString(double _frequency)
: freq(_frequency), excitationSrc(NULL)
{
	samplingRate = (int)floor(44100 / freq - 0.5); // round to nearest integer
	for (int i = 0; i < samplingRate; ++i)
		//buffer.push(0);
		buffer.push_back(0);
}

PluckedString::PluckedString(double* _buffer, int n)
{
	samplingRate = n;
	for (int i = 0; i < n; ++i)
		buffer.push_back(_buffer[i]);
}

PluckedString::~PluckedString() {
}

void PluckedString::pluck() {
	buffer.clear();

	if (excitationSrc) {
		buffer = excitationCache;
		/*
		srand(time(NULL));
		//srand(1307);
		
		// generate excitation signal
		switch (param.excitationType) {
		case WhiteNoise:
		{
			for (int i = 0; i < samplingRate; ++i)
				buffer.push_back(random(-param.amplitude, param.amplitude) * excitationCache[i]);
			break;
		}
		}*/
	} else {
		srand(time(NULL));
		//srand(1307);
		
		// generate excitation signal
		switch (param.excitationType) {
		case WhiteNoise:
		{
			for (int i = 0; i < samplingRate; ++i)
				buffer.push_back(random(-param.amplitude, param.amplitude));
			break;
		}
		}
	}
}

void PluckedString::setExcitation(Excitation* ext) {
	excitationSrc = ext;

	// preload excitation cache
	for (int i = 0; i < samplingRate; ++i) {
		excitationCache.push_back(excitationSrc->sample() * param.amplitude);
		excitationSrc->tick();
	}
}

void PluckedString::tic() {
	//
	// Karplus-Strong plucked string model 
	//	
	// Why a ring buffer implementation is memory efficient.
	// In real-time synthesis, it is not necessary to perform the filter to all samples in the buffer like in image processing. 
	// When a sample is played (sent to audio output), it can be discarded from the buffer (for FIR filter), or queued back (for IIR filter). 
	// The next sample in the buffer is then filtered and ready to be sent to audio output in the next time step. 
	//double y0 = buffer.front(); buffer.pop();
	//double y1 = buffer.front();
	
	double gain = param.gain; // am vang
	double y0 = buffer[0];
	double y1 = buffer[1];
	//double y2 = buffer[2];
	buffer.push_back(0.5 * (y0 + y1) * gain);
	//buffer.push_back(0.33 * (y0 + y1 + y2) * gain);
	buffer.pop_front();
}

int PluckedString::timeElapsed() {
	return 0;
}

double PluckedString::sample() {
	return buffer.front();
}

double PluckedString::maxSample() {
	double maxVal = DBL_MIN;
	for (int i = 0; i < buffer.size(); ++i) {
		maxVal = max(maxVal, fabs(buffer[i]));
	}
	return maxVal;
}

double PluckedString::frequency() const {
	return freq;
}

void PluckedString::setPluckedStringParameter(PluckedStringParameter* _param) {
	this->param = *_param; // copy parameter settings
}

const PluckedStringParameter* PluckedString::getPluckedStringParameter() const {
	return &param;
}

}
