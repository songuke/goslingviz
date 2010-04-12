#ifndef _GOS_FILE_MANAGER_
#define _GOS_FILE_MANAGER_

#include "GosMustHave.h"
#include <vector>

/**
 AntTweakBar library to make a simple UI for parameter tweak.
 */
#include <AntTweakBar/AntTweakBar.h>

namespace Gos {
class FileManager
{
public:
	FileManager(void);
	~FileManager(void);
		
	void init();
	void addFile(const String& file);
	void addPlaylist(const String& file);

	/**
	Use AntTweakBar library to render file list in OpenGL.
	*/
	void render();
	void onMouse(int button, int state, int x, int y);
	void onMotion(int x, int y);
	void onPassiveMotion(int x, int y);
	void onKeyboard(unsigned char key, int x, int y);
	void onSpecial(int key, int x, int y);
	void onTerminate();
	void onResize(int width, int height);

	/**
	Load current file list to TweakBar UI.
	*/
	void loadFileListToTweakBar();	
	
	/**
	Return current (playing) file index.
	*/
	//int getCurrentFileIndex();

	/**
	Register an object to notify when the playing music file is changed.
	*/
	void observerFileChangedFor(FileChangedHandler* in);
	void removeObserveFileChangedFor(FileChangedHandler* in);
protected:
	TwBar *bar;						// pointer to the tweak bar
	int		loadAntTweakBar();
protected:
	std::vector<String> files;
	std::vector<char*> filesNamePtr;
	TwEnumVal* filesEV;
	int curFileIndex, lastFileIndex;
	typedef std::vector<FileChangedHandler*> FileChangedHandlerList;
	FileChangedHandlerList fileChangedObservers;
};
}

#endif