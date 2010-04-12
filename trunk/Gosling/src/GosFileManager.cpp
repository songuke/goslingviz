#include "GosFileManager.h"

namespace Gos
{
FileManager::FileManager(void)
: filesEV(0), curFileIndex(0), lastFileIndex(-1)
{
}

FileManager::~FileManager(void)
{
	safeDel(filesEV);

	// remove all allocated string when loading file list to AntTweakBar
	for (int i = 0; i < filesNamePtr.size(); ++i) {
		char* buf = filesNamePtr[i];
		delete buf;
	}	
}

void FileManager::init() {
	this->loadAntTweakBar();	
}

void FileManager::addFile(const String& file) {
	files.push_back(file);
}

void FileManager::addPlaylist(const String& file) {
	//
	// File format: text file.
	// Each line contains a relative file path and file name.
	// The file must contain an empty ending line.
	//
	FILE* f = fopen(file.c_str(), "r");
	const int bufLen = 256;
	char buf[bufLen];
	while (!feof(f)) {
		fgets(buf, bufLen, f);
		if (feof(f)) break;

		// remove end line character
		char* p = buf;
		while (*p != '\r' && *p != '\n' && *p != '\0') ++p;
		*p = '\0';
		String str = buf;
		// trim beginning and endling space
		std::remove(str.begin(), str.end(), ' ');

		if (str.empty()) continue;
		this->addFile(str);	
	}
	fclose(f);
}

/*
int	FileManager::getCurrentFileIndex() {
	return curFileIndex;
}*/

void FileManager::render() {
	// draw tweak bars
    TwDraw();
	// 
	// keep track of file indices changes
	//
	if (lastFileIndex != curFileIndex) {
		//
		// notify
		//
		for (int i = 0; i < fileChangedObservers.size(); ++i) {
			fileChangedObservers[i]->onFileChanged(files[curFileIndex]);
		}
	}
	lastFileIndex = curFileIndex;
}

void FileManager::observerFileChangedFor(FileChangedHandler* in) {
	fileChangedObservers.push_back(in);
}

void FileManager::removeObserveFileChangedFor(FileChangedHandler* in) {
	FileChangedHandlerList::iterator i;
	for (i = fileChangedObservers.begin(); i != fileChangedObservers.end(); ++i) {
		if (*i == in) {
			break;
		}
	}
	fileChangedObservers.erase(i);
}

void FileManager::onMouse(int button, int state, int x, int y) {
	TwEventMouseButtonGLUT(button, state, x, y);
}

void FileManager::onMotion(int x, int y) {
	TwEventMouseMotionGLUT(x, y);
}

void FileManager::onPassiveMotion(int x, int y) {
	TwEventMouseMotionGLUT(x, y);
}

void FileManager::onKeyboard(unsigned char key, int x, int y) {
	TwEventKeyboardGLUT(key, x, y);
}

void FileManager::onSpecial(int key, int x, int y) {
	TwEventSpecialGLUT(key, x, y);
}

void FileManager::onTerminate() {
	TwTerminate();
}

void FileManager::onResize(int width, int height) {
	// Send the new window size to AntTweakBar
    TwWindowSize(width, height);
}

/**
 Do necessary initialization to AntTweakBar library.
 */
int FileManager::loadAntTweakBar() {
	// Initialize AntTweakBar
    // (note that AntTweakBar could also be intialized after GLUT, no matter)
    if( !TwInit(TW_OPENGL, NULL) )
    {
        // A fatal error occured    
        fprintf(stderr, "AntTweakBar initialization failed: %s\n", TwGetLastError());
        return 1;
    }
	// - Send 'glutGetModifers' function pointer to AntTweakBar;
    //   required because the GLUT key event functions do not report key modifiers states.
    TwGLUTModifiersFunc(glutGetModifiers);

    // Create a tweak bar
    bar = TwNewBar("Playlist");
    //TwDefine(" GLOBAL help='' "); // Message added to the help bar.
    TwDefine(" Playlist size='280 440' color='128 128 128' position='200 20' valueswidth=360 fontsize=2 "); // change default tweak bar size and color

	// Add the enum variable 'g_CurrentShape' to 'bar'
    // (before adding an enum variable, its enum type must be declared to AntTweakBar as follow)
	/*
    {

        // ShapeEV associates Shape enum values with labels that will be displayed instead of enum values
        TwEnumVal shapeEV[nbFiles] = { {SHAPE_TEAPOT, "Teapot"}, {SHAPE_TORUS, "Torus"}, {SHAPE_CONE, "Cone"} };
        // Create a type for the enum shapeEV
        TwType shapeType = TwDefineEnum("ShapeType", shapeEV, NUM_SHAPES);
        // add 'g_CurrentShape' to 'bar': this is a variable of type ShapeType. Its key shortcuts are [<] and [>].
        TwAddVarRW(bar, "Shape", shapeType, &g_CurrentShape, " keyIncr='<' keyDecr='>' help='Change object shape.' ");
    }*/

	return 0;
}

void FileManager::loadFileListToTweakBar() {
	int nbFiles = files.size();
	filesEV = new TwEnumVal[nbFiles];
	for (int i = 0; i < nbFiles; ++i) {
		int len = strlen(files[i].c_str());
		char* buffer = new char[len + 1];
		strcpy(buffer, files[i].c_str());
		filesNamePtr.push_back(buffer);	
		filesEV[i].Label = buffer;
		filesEV[i].Value = i;
	}
	TwType fileType = TwDefineEnum("FileType", filesEV, nbFiles);
	TwRemoveVar(bar, "Files");
	TwAddVarRW(bar, "Files", fileType, &curFileIndex, " keyIncr='<' keyDecr='>' ");
}

}
