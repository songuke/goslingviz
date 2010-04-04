#ifndef _GOS_GUITAR_KEYBOARD_
#define _GOS_GUITAR_KEYBOARD_

#include "GosMustHave.h"

#include "GosPluckedString.h"
#include "GosExcitation.h"
#include "GosGuitarBody.h"

#include <deque>
#include <map>
using namespace std;

namespace Gos
{
class GuitarKeyboardParameter : Parameter
{
public:
	PluckedStringParameter string;
	unsigned int minimumTimePlay;
	double thresRemove;
	double thresLowSound;
	double thresSilent;
};

class GuitarKeyboard
{
public:
	GuitarKeyboard(void);
	~GuitarKeyboard(void);

public:
	void	pluck(char key);
	void	pluck(const String& keys);						// set of 6 keys in a chord
	void	pluck(double freq);
	void	pluck(double* freqs, int n);

	double	sample();
	void	tic();
	bool	isDonePlaying();
protected:
	void	updatePluck();									// queue offset = 0 key into playing queue
protected:	
	PluckedString** strings;
	int nbStrings;
	double curSample;										// current sample value
	typedef std::map<char, PluckedString*> StringMap;		// mapping of key to pluckable string
	StringMap map;
	typedef std::deque<PluckedString*> StringQueue;			// queue of plucked strings
	StringQueue queue;


	typedef void (*FreqChangeCallback)(double* newFreq, int n);
	FreqChangeCallback freqCallback;

	double	getFrequencyCount();							// return the number of frequencies now playing
	void	getFrequencies(double* freqs);					// return the frequencies now playing
	void	notifyFrequenciesChanged();						// fire frequenciesChanged event. 
	
public:
	/**
	Register a frequencies changed callback function
	*/
	void	onFrequenciesChanged(FreqChangeCallback c);

public:
	void							setGuitarKeyboardParameter(GuitarKeyboardParameter* _param);
	const GuitarKeyboardParameter*	getGuitarKeyboardParameter() const;
protected:
	GuitarKeyboardParameter param;

public:
	static String keys;										// key table
	
	static char		freq2char(double freq);
	static double	char2freq(char c);
protected:
	enum State {
		Playing,
		Stopped,
		LowSound,		// ready to get new keys to play
		Silenting		// silent and let samples fades out
	} state;
	void changeState(State _state);
	long timeElapsed;
	static int		stringFrameOffset[6];					// offset of string frames when played together in a chord
	String	pendingKeys;									// key (chord) waiting for its correct time offset to move into queue for playing
	int		pendingOffset[6];
	int		pendingCount;

	//double thresRemove;										// silent threshold
	//double thresLowSound;
	//double thresSilent;
	double countSilent;

	//int minimumTimePlay;									// minimum life time of a note. A high frequency note should not be cut too early.

	double stringPrevSample[6];
	double stringDiffTimeAvg[6];							// differential value of samples over a period of time. If this value falls below a threshold, it's time to remove the playing note.

	double prevSample;
	double diffTimeAvg;
public:
	bool isStopped();
	bool isPlaying();
	bool isLowSound();										// LowSound state allows new keys to be added to play

protected:
	static String fileBody;
	static String fileBodyBiQuad;
	GuitarBody body;										// body filter
	
	static String fileExcitation;
	Excitation excitation;									// excitation source

};
}

#endif
