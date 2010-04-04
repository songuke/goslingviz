#include "GosMustHave.h"
#include "GosGuitarPlayer.h"
#include "GosGuitarKeyboard.h"


namespace Gos
{
double	GuitarPlayer::stringFrequency[] = {
	329.6, // ear
	246.9, // big
	196.0, // grow
	146.8, // dog
	110.0, // and
	82.4   // elephant
};
String GuitarPlayer::empty = "------";

GuitarPlayer::GuitarPlayer(void)
{
	reset();
}

GuitarPlayer::~GuitarPlayer(void)
{
}

char GuitarPlayer::word2key(String word, int string) {
	String number;
	if (! (	('a' <= word[0] && word[0] <= 'z') ||
			('A' <= word[0] && word[0] <= 'Z')
			)) {
		number = word;
	} else {
		number = word.substr(1);
	}
	int fret = atoi(number.c_str());
	//double freq = stringFrequency[string] * pow(1.0595, fret);
	double freq = stringFrequency[string] * pow(2, fret / 12.0);
	return GuitarKeyboard::freq2char(freq);
}

void GuitarPlayer::loadTablatureText(const String& file) {
	FILE* f;
	f = fopen(file.c_str(), "r");
	if (f == NULL) {
		//throw_exception(new boost::exception(""));
		//throw FileReadError() << ErrnoInfo(errno);
		FileReadError e;
		e << ErrnoInfo(errno) << FileNameInfo(file);
		throw e;
	}

	int nbChars = 32;
	char* segment = new char[nbChars];

	int row = 0;
	int col = 0;	
	const int wordLength = 4;
	char word[wordLength];
	int j = 0;
	bool hasWord = false;
	
	while (feof(f) == 0) {
		segment[0] = '\0'; // reset
		fgets(segment, nbChars, f);		
		//if (feof(f)) {
			// remove eof char
			//segment[strlen(segment)] = '\0';
		//}
		for (int i = 0; i < strlen(segment); ++i) {			 
			if (row == 0 && segment[i] != '\r' && segment[i] != '\n') { // create table
				tabs.push_back(empty);
				//printf("Char %c, col = %d\n", segment[i], col);
			}

			if (segment[i] == '-' || segment[i] == '\r' || segment[i] == '\n') {
				if (hasWord) {
					String& tab = tabs[col - 1];
					tab[row] = word2key(word, row);

					memset(word, 0, wordLength);
					hasWord = false;
					j = 0;
				}
				if (segment[i] == '\r' || segment[i] == '\n') { // new line found
					++row;
					col = 0;				
					//memset(word, 0, wordLength);
					//hasWord = false;
					//j = 0;
					break;
				}
			} else {
				// add this character to a temporary buffer
				hasWord = true;
				word[j] = segment[i];
				++j;
			}				
			++col;			
		}
	}

	/*
	// remove empty rows
	std::vector<Tablature::iterator> removable;
	for (Tablature::iterator i = tabs.begin(); i != tabs.end(); ++i)
		if ((*i).compare(empty) == 0) // is empty
			removable.push_back(i);
	for (int i = removable.size() - 1; i >= 0; --i)
		tabs.erase(removable[i]);

	fclose(f);

	// print
	for (int i = 0; i < tabs.size(); ++i) {
		printf("%s\n", tabs[i].c_str());
	}*/
}

String GuitarPlayer::sample() {
	return tabs[tabIdx];
}

void GuitarPlayer::tic() {
	++tabIdx;
}

bool GuitarPlayer::hasNext() {
	return tabIdx < tabs.size();
}

void GuitarPlayer::reset() {
	tabIdx = 0;
}

}
