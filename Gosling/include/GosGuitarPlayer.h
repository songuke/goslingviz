#ifndef _GOS_GUITAR_PLAYER_
#define _GOS_GUITAR_PLAYER_

#include "GosMustHave.h"
#include <vector>
using namespace std;

namespace Gos
{
class GuitarPlayer
{
public:
	GuitarPlayer(void);
	~GuitarPlayer(void);

public:
	/**
	Guitar tablature file format: the common notation of tablature
	stored in text format. There are six rows corresponding to 6 strings. 
	The top string is the first string. The number denotes the fret of the corresponding string.
	--10--9--7--5----10--9--7--5------------------------------------
	------------------------------5---5--7--8--7--5--3----0--3------
	----------------------------------------------------------------
	----------------------------------------------------------------
	----------------------------------------------------------------
	----------------------------------------------------------------
	*/
	void loadTablatureText(const String& file);
	
	String sample();		// return a set of 6 notes to play
	void tic();				// advance to next frame
	bool hasNext();
	void reset();
protected:
	/**
	Convert the note on input fret and string to corresponding character reprsentation
	*/
	char word2key(String fret, int string);

protected:
	typedef std::vector<String> Tablature;
	Tablature tabs;
	int tabIdx;								// current tab index
	static double	stringFrequency[6];		// open string frequency
public:
	static String	empty;
};
}

#endif
