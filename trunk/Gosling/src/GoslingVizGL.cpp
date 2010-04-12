//-----------------------------------------------------------------------------
// OpenGL audio player and visualization.
// Control UI by AntTweakBar UI library and CEGUI library.
//
// Hua Binh Son
// Yang Jianqing
//
// 2010-03-25
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Gosling
//-----------------------------------------------------------------------------
#include "GosMustHave.h"
#include "GosChunk.h"
#include "GosAudioIn.h"
#include "GosAudioOut.h"

#include "GosFileManager.h"

#include "GosOscilloscope.h"
#include "GosSpectrogram.h"
#include "GosSpectrumCircle.h"
#include "GosCurveWarp.h"
#include "GosGlobalVisualizer.h"
#include "RenzoTimer.h"

using namespace Gos;
using namespace Renzo;

/**
 Boost.Thread library
 */
#include <boost/thread.hpp>

//-----------------------------------------------------------------------------
// Window
//-----------------------------------------------------------------------------
int winWidth	=	512;
int winHeight	=	512;

//-----------------------------------------------------------------------------
// Audio
//-----------------------------------------------------------------------------
Chunk* chunk;
boost::mutex mutexChunk;		// for thread safe read/write to chunk
bool	isPlaying = false;

//-----------------------------------------------------------------------------
// Rendering
//-----------------------------------------------------------------------------
float zNear = -320.0f;
float zFar  = 320.0f;
Visualizer* visualizer = 0;
Rect		rectVis;			// visualization rect in the window

Timer timer;

//-----------------------------------------------------------------------------
// Audio
//-----------------------------------------------------------------------------
AudioIn audioIn;
AudioOut audioOut;
String fileAudio;

// playback control
int			audioFrameTime		= 0;
int			visualFrameTime		= 0;
const float	audioFPS			= kSampleRate / kChunkSize;
const int	idealFrameTime		= 1000 / audioFPS;
Timer		audioTimer;
Timer		visualTimer;
FileManager fileManager;
//-----------------------------------------------------------------------------
// Forward declaration
//-----------------------------------------------------------------------------
void	renderString(float x, float y, 
					 void *font, const char* string, Float4 const& rgb);
void	play();

//-----------------------------------------------------------------------------
/**
 Initialization. Called once when OpenGL starts.
 */
void init() {
	//
	// AntTweakBar
	//	
	fileManager.init();
	// add files
	//fileManager.addFile("music/The_Sad_Rose.wav");
	//fileManager.addFile("music/classical.wav");
	fileManager.addPlaylist("music/playlist.txt");
	fileManager.loadFileListToTweakBar();

	chunk = new Chunk();
	//audioIn.loadAudio(fileAudio);
	//audioOut.setChannels(audioIn.getChannels());
	audioOut.setChannels(2);
	fileManager.observerFileChangedFor((FileChangedHandler*)&audioIn);

	//visualizer = new Oscilloscope();
	//visualizer = new Spectrogram();
	visualizer = new CurveWarp();	
	audioIn.observeBeatFor(visualizer);
	fileManager.observerFileChangedFor((FileChangedHandler*)visualizer);
	//visualizer = new SpectrumCircle();
	//visualizer = new GlobalVisualizer();
}


void cleanUp() {
	safeDel(chunk);
	safeDel(visualizer);
}

// callback function called by GLUT to render screen
void display(void)
{
	// timer update
	int delta = timer.Update();	

	// clear frame buffer
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	visualTimer.Reset();
	visualTimer.Update();
	if (isPlaying) {
		// visualization	
		try {
			visualizer->update(delta);
			visualizer->render(*chunk, rectVis);
		} catch (std::exception& e) {
			printf("Exception: %s\n", e.what());
		}
	}
	visualTimer.Update();
	visualFrameTime = visualTimer.GetTimeElapsed();

	// render file list
	fileManager.render();

	// draw FPS
	char fps[16];
	sprintf(fps, "%.2f", timer.GetAverageFps());
	renderString(0.0f, 0.0f, GLUT_BITMAP_TIMES_ROMAN_24, fps, Float4(1.0f, 1.0f, 1.0f, 1.0f));

    // present frame buffer
    glutSwapBuffers();
}

void idle() {	
	audioTimer.Reset();
	audioTimer.Update();
	// play audio first
	if (isPlaying) {
		if (audioIn.hasNext()) {
			// get a chunk
			//mutexChunk.lock();
			audioIn.sampleChunk(*chunk);		
			//mutexChunk.unlock();

			// send to output
			audioOut.tickChunk(*chunk);
			
			// sleep
			//sleepMiliseconds(1);
		} else {
			isPlaying = false;
		}
	} else {
		audioOut.stop();
	}
	audioTimer.Update();
	audioFrameTime = audioTimer.GetTimeElapsed();

	// decide to render or skip frame
	if (visualFrameTime <= idealFrameTime - audioFrameTime) 
		// render
		glutPostRedisplay();
	else
		//visualFrameTime -= audioFrameTime;
		visualFrameTime = 0;

	// sleep
	sleepMiliseconds(1);
}

void renderString(float x, float y, void *font, const char* str, Float4 const& rgb)
{  
	glPushAttrib(GL_ENABLE_BIT);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_DEPTH_TEST);
		
		int len = strlen(str);
		
		glColor4f(rgb.x, rgb.y, rgb.z, rgb.w); 
		/*
		glRasterPos2i(x+1, y+1);		
		for (int i = 0; i < len; i++) {
			glutBitmapCharacter(GLUT_BITMAP_8_BY_13, str[i]);
		}
		
		glColor3f(1, 1, 1); //white
		*/
		glRasterPos2i(x, y);				
		for (int i = 0; i < len; i++) {
			//Draw		
			glutBitmapCharacter(GLUT_BITMAP_8_BY_13, str[i]);
		}
	glPopAttrib();
}



// callback function called by GLUT when window size changes
void reshape(int width, int height)
{
	winWidth	= width;
	winHeight	= height;
	// TODO: in order to support rectangle window, the texture loaded from file should be scaled to 
	// match the window size. This is not done yet.
	//winWidth = winHeight = mmin(width, height); 


    // Set OpenGL viewport and camera
    glViewport(0, 0, winWidth, winHeight);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, winWidth, 0, winHeight, zNear, zFar);

	glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	// set visualization rect to be full window
	rectVis.left	= 0.0f;
	rectVis.right	= winWidth;
	rectVis.top		= winHeight;
	rectVis.bottom	= 0.0f;

	fileManager.onResize(width, height);
}

void mouse(int button, int state, int x, int y) {
	fileManager.onMouse(button, state, x, y);
}

void motion(int x, int y) {
	fileManager.onMotion(x, y);
}

void passiveMotion(int x, int y) {
	fileManager.onPassiveMotion(x, y);
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
		case ' ':
			isPlaying = 1 - isPlaying;
			break;		
	}
	visualizer->onKey(key);
	fileManager.onKeyboard(key, x, y);
}

void special(int key, int x, int y) {
	fileManager.onSpecial(key, x, y);
}

// function called at exit
void termination(void)
{ 
	fileManager.onTerminate();  
}

void play() {
	//if (isPlaying) return;

	//isPlaying = true;
	// use Boost.Thread to create a thread for audio playback
	//boost::thread threadAudio(mainAudio);
}

void printUsage() {
	printf("--------------------------------------------------------------------------------\n");
	printf("CS5249 Audio Visualization\n");
	printf("--------------------------------------------------------------------------------\n");
}
 
int main(int argc, char *argv[])
{   
	// information
	printUsage();

	// initialize GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(winWidth, winHeight);
    glutCreateWindow("CS5249 Audio Visualization");
    glutCreateMenu(NULL);

    // set GLUT callbacks
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutPassiveMotionFunc(passiveMotion);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);
	glutIdleFunc(idle);
    
    // called after glutMainLoop ends
	atexit(termination);  
	
	// prepare a random generator
	srand(time(NULL));

	// initialize
	init();	

    // call the GLUT main loop
    glutMainLoop();

    return 0;
}

