#include "GosMustHave.h"

#include "GosExcitation.h"
#include "GosPluckedString.h"
#include "GosGuitarKeyboard.h"
#include "GosGuitarPlayer.h"

#include <vector>
using namespace std;

namespace Gos
{
String	GuitarKeyboard::keys		= "1234567890qwertyuiopasdfghjklzxcvbnm,|QWERTY";
int		GuitarKeyboard::stringFrameOffset[] = {
			24, //19,		// Ear
			19, //14,		// Big
			15, //10,		// Grow
			10, //5,		// Dog
			5,  //0,		// And
			0,  //-5		// Elephant
		};
String	GuitarKeyboard::fileBody		= "./output/gtrbody_residual.wav";
String	GuitarKeyboard::fileExcitation	= "./output/gtrbody_excitation.wav";
String	GuitarKeyboard::fileBodyBiQuad  = "./output/gtrbody_BA.txt";

GuitarKeyboard::GuitarKeyboard(void)
: freqCallback(0), timeElapsed(0), state(LowSound)
{	
	excitation.loadExcitationSource(fileExcitation);

	nbStrings = keys.length();
	strings = new PluckedString*[nbStrings];
	for (int i = 0; i < nbStrings; ++i) {
		double freq = 440 * pow(2, (i - 29) / 12.0);
		strings[i] = new PluckedString(freq);
		map[keys[i]]= strings[i];
		printf("%c\t %.2fHz\n", keys[i], freq);

		strings[i]->setExcitation(&excitation);
	}

	//body.loadBodyResonance(fileBody);
	body.loadBodyBiQuad(fileBodyBiQuad);

	memset(stringDiffTimeAvg, 0, sizeof(double) * 6);
	memset(stringPrevSample, 0, sizeof(double) * 6);
	diffTimeAvg = 0;
	prevSample = 0;
}

GuitarKeyboard::~GuitarKeyboard(void)
{
	for (int i = 0; i < nbStrings; ++i)
		delete strings[i];
	delete[] strings;
}

char GuitarKeyboard::freq2char(double freq) {
	int i = (int)floor((log2(freq / 440) * 12 + 29) + 0.5);
	return keys[i];
}

double GuitarKeyboard::char2freq(char c) {
	int i = keys.find(c);
	return 440 * pow(2, (i - 29) / 12.0);
}

void GuitarKeyboard::pluck(char key) {
	//queue.clear();
	this->changeState(Playing);

	StringMap::iterator i = map.find(key);
	if (i != map.end()) {
		PluckedString* s = i->second;
		queue.push_back(s);
		s->pluck();
	}

	notifyFrequenciesChanged();
}

void GuitarKeyboard::pluck(const String& keys) {
	if(keys.length() != 6) {
		throw std::exception("Keys count is not equal to 6.");
	}

	// reset time counter
	timeElapsed = 0;

	//queue.clear();
	
	if (keys.compare(GuitarPlayer::empty) == 0) {
		this->changeState(Silenting);		
	} else {

		this->changeState(Playing);
		memcpy(pendingOffset, stringFrameOffset, sizeof(pendingOffset));
		// Find the first key from the bottom (6th string) up and mark its offset to 0. 
		// Keys above the first key are offset relatively from it.
		for (int i = keys.length() - 1; i >= 0; --i) {
			StringMap::iterator iter = map.find(keys[i]);
			if (iter == map.end()) // key not playable			
				pendingOffset[i] = -1;
			else {
				// the first playable key
				pendingOffset[i] = 0;
				for (int j = i + 1; j < keys.length(); ++j) {
					pendingOffset[j] -= pendingOffset[i];
				}
				break;			
			}
		}
		pendingKeys.clear();
		pendingKeys.append(keys);
		pendingCount = keys.length();
	}
}

void GuitarKeyboard::pluck(double freq) {
	this->pluck(freq2char(freq));
}

void GuitarKeyboard::pluck(double* freqs, int n) {
	char* c = new char[n];
	for (int i = 0; i < n; ++i) {
		c[i] = freq2char(freqs[i]);
	}
	String s = c;
	this->pluck(s);
	delete[] c;
}

/**
 Put key into queue when offset reaches 0
 */
void GuitarKeyboard::updatePluck() {	
	if (state == Silenting) return;
	if (pendingCount <= 0) return;

	bool needNotify = false;
	for (int i = 0; i < pendingKeys.length(); ++i) {
		if (pendingOffset[i] == 0) {
			StringMap::iterator iter = map.find(pendingKeys[i]);
			if (iter != map.end()) {			
				PluckedString* s = iter->second;
				queue.push_back(s);
				s->pluck();
				--pendingCount;
				needNotify = true;

				// update cutoff threshold
				//cutoff = max(cutoff, s->maxSample() * cutoffCoef);
			}
		}
		--pendingOffset[i];
	}
	if (needNotify)
		notifyFrequenciesChanged();
}

void GuitarKeyboard::tic() {
	++timeElapsed;

	this->updatePluck();

	//StringQueue::reverse_iterator i;
	//typedef std::vector<StringQueue::iterator> Removable;
	//Removable removable;
	
	bool needNotify = false;
	//for (i = queue.rbegin(); i != queue.rend(); ++i) {
	for (int i = queue.size() - 1; i >= 0; --i) {
		queue[i]->tic();
		double sample = queue[i]->sample();
		
		stringDiffTimeAvg[i] = stringDiffTimeAvg[i] * 0.7 + fabs(sample - stringPrevSample[i]) * 0.3;
		stringPrevSample[i] = sample;

		//if (fabs(sample) < thresRemove) {
		if (stringDiffTimeAvg[i] < param.thresRemove) {
			//removable.push_back(i);
			queue.erase(queue.begin() + i);

			needNotify = true;
		}
	}
	/*
	bool needNotify = removable.size() > 0;
	
	Removable::iterator j;
	for (j = removable.end(); j != removable.begin(); --j) { // remove from end to begin to avoid index changes
		queue.erase(*j);
	}
	*/
	if (needNotify)
		notifyFrequenciesChanged();

	switch (state) {
		case Silenting:
			if (countSilent == 0) {
				//queue.clear();
				this->changeState(LowSound);
			}
			--countSilent;
			break;
	}

	//
	// Compute sample
	//
	StringQueue::iterator i;
	curSample = 0;
	for (i = queue.begin(); i != queue.end(); ++i) {
		curSample += (*i)->sample();
	}
	
	//
	// integrate body filter
	//
	body.tic(curSample);
	curSample = body.sample();
	
	// clamp 
	curSample = min(max(curSample, -param.string.amplitude), param.string.amplitude);
	diffTimeAvg = 0.7 * diffTimeAvg + 0.3 * fabs(curSample - prevSample);
	prevSample = curSample;

	// state management
	switch (state) {
		case Silenting:
			break;
		default:
			// do not allow to cut too early. // cut when a sample is near to zero.
			//if (timeElapsed > param.minimumTimePlay && fabs(curSample) < thresLowSound)
			if (timeElapsed > param.minimumTimePlay && diffTimeAvg < param.thresLowSound)
				//changeState(Stopped);
				changeState(LowSound);
			break;
	}
}

/*
double GuitarKeyboard::sample() {
	StringQueue::iterator i;
	double sample = 0;
	for (i = queue.begin(); i != queue.end(); ++i) {
		sample += (*i)->sample();
	}
	// clamp 
	sample = min(max(sample, -param.string.amplitude), param.string.amplitude);
	
	switch (state) {
		case Silenting:
			break;
		default:
			// do not allow to cut too early. // cut when a sample is near to zero.
			if (timeElapsed > minimumTimePlay && fabs(sample) < thresLowSound)
				//changeState(Stopped);
				changeState(LowSound);
			break;
	}

	return sample;
}*/


double GuitarKeyboard::sample() {
	return curSample;
}

double GuitarKeyboard::getFrequencyCount() {
	return queue.size();
}

void GuitarKeyboard::getFrequencies(double* freqs) {
	StringQueue::iterator i;	
	int n = 0;
	for (i = queue.begin(); i != queue.end(); ++i) {
		freqs[n] = (*i)->frequency();
		++n;
	}
}

void GuitarKeyboard::onFrequenciesChanged(FreqChangeCallback c) {
	freqCallback = c;
}

void GuitarKeyboard::notifyFrequenciesChanged() {
	if (freqCallback) {
		// notify frequency change
		double* freqs = new double[keys.length()];
		int nbFreqs = getFrequencyCount();
		getFrequencies(freqs);
		(*freqCallback)(freqs, nbFreqs);
		delete[] freqs;
	}
}

void GuitarKeyboard::changeState(State _state) {
	state = _state;
	switch(state) {
		case Silenting:
			countSilent = param.thresSilent;
			break;
	}
}

bool GuitarKeyboard::isStopped() {
	return state == Stopped;
}

bool GuitarKeyboard::isPlaying() {
	return state == Playing;
}

bool GuitarKeyboard::isLowSound() {
	return state == LowSound;
}

void GuitarKeyboard::setGuitarKeyboardParameter(GuitarKeyboardParameter* _param) {
	this->param = *_param;

	// update all string parameters
	for (int i = 0; i < nbStrings; ++i) {
		strings[i]->setPluckedStringParameter(&param.string);
	}
}

const GuitarKeyboardParameter* GuitarKeyboard::getGuitarKeyboardParameter() const {
	return &param;
}


}
