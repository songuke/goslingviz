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
bool	renderVisualizer;
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
void	initVisualizer();
void	switchVisualizer();
void	loadVisualizerToTweakBar();
int		loadAntTweakBar();

//-----------------------------------------------------------------------------
// GUI
//-----------------------------------------------------------------------------
TwBar*	bar;						// pointer to the tweak bar

enum VisualizerEnum {
	VisOscilloscope,
	VisSpectrogram,
	VisCircularSpectrogram,
	VisCurveWarp,
	VisGlobalVisualizer,
	nbVisualizers
};
int		curVisualizer = VisCurveWarp;
int		lastVisualizer = -1;
//-----------------------------------------------------------------------------
/**
 Initialization. Called once when OpenGL starts.
 */
void init() {	
	// AntTweakBar	
	loadAntTweakBar();

	// add file list
	fileManager.init();		
	//fileManager.addFile("music/classical.wav");
	fileManager.addPlaylist("music/playlist.txt");
	fileManager.loadFileListToTweakBar(bar);

	chunk = new Chunk();
	//audioIn.loadAudio(fileAudio);
	//audioOut.setChannels(audioIn.getChannels());
	audioOut.setChannels(2);
	fileManager.observerFileChangedFor((FileChangedHandler*)&audioIn);
	
	// add visualizer list
	loadVisualizerToTweakBar();
}

void initVisualizer() {
	if (visualizer) {
		audioIn.observeBeatFor(visualizer);
		fileManager.observerFileChangedFor((FileChangedHandler*)visualizer);
	}
}

void removeVisualizer() {
	if (visualizer) {
		audioIn.removeObserveBeatFor(visualizer);
		fileManager.removeObserveFileChangedFor((FileChangedHandler*)visualizer);
		safeDel(visualizer);
	}
}

void switchVisualizer() {
	if (lastVisualizer == curVisualizer) return;

	// remove current visualizer
	removeVisualizer();

	int i = curVisualizer;
	switch (i) {
	case 0:
		visualizer = new Oscilloscope();
		break;
	case 1:
		visualizer = new Spectrogram();
		break;
	case 2:
		visualizer = new SpectrumCircle();
		break;
	case 3:
		visualizer = new CurveWarp();
		break;
	case 4:
		visualizer = new GlobalVisualizer();
		break;
	}

	initVisualizer();
	lastVisualizer = curVisualizer;	
}

void loadVisualizerToTweakBar() {
	// ShapeEV associates Shape enum values with labels that will be displayed instead of enum values
    TwEnumVal visEV[5] = {	{VisOscilloscope, "Oscilloscope"}, 
							{VisSpectrogram, "Spectrogram"},
							{VisCircularSpectrogram, "Circular Spectrogram"},
							{VisCurveWarp, "Curve Warp"},
							{VisGlobalVisualizer, "Global Visualizer"}
	};

    // Create a type for the enum shapeEV
    TwType visType = TwDefineEnum("VisType", visEV, nbVisualizers);    
    TwAddVarRW(bar, "Visualizer", visType, &curVisualizer, "");
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
	
	// switch visualizer if any
	switchVisualizer();

	// clear frame buffer
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	visualTimer.Reset();
	visualTimer.Update();
	if (renderVisualizer) {
	if (isPlaying) {
		// visualization	
		//try {			
			visualizer->update(delta);
			visualizer->render(*chunk, rectVis);
		//} catch (std::exception& e) {
		//	printf("Exception: %s\n", e.what());
		//}
	}
	}
	
	// render file list
	//fileManager.render();
	TwDraw();

	// draw FPS
	char fps[16];
	sprintf(fps, "%.2f", timer.GetAverageFps());
	renderString(0.0f, 0.0f, GLUT_BITMAP_TIMES_ROMAN_24, fps, Float4(1.0f, 1.0f, 1.0f, 1.0f));

	visualTimer.Update();
	visualFrameTime = visualTimer.GetTimeElapsed();

    // present frame buffer
    glutSwapBuffers();
}

void idle() {	
	fileManager.update();

	audioTimer.Reset();
	audioTimer.Update();
	// play audio first
	if (isPlaying) {
		if (audioIn.hasNext()) {
			// get a chunk
			//mutexChunk.lock();
			audioIn.sampleChunk(*chunk);		
			//mutexChunk.unlock();
			
			// sleep
			//sleepMiliseconds(1);
		} else {
			isPlaying = false;
			//audioOut.stop();
			//memset(chunk, 0, sizeof(Chunk));
		}
	} else {
		//audioOut.stop();
		memset(chunk, 0, sizeof(Chunk));
	}
	// send to output
	audioOut.tickChunk(*chunk);

	audioTimer.Update();
	audioFrameTime = audioTimer.GetTimeElapsed();

	// decide to render or skip frame
	if (visualFrameTime <= idealFrameTime - audioFrameTime) {
		renderVisualizer = true;		
		display();		
	} else {
		//visualFrameTime -= audioFrameTime;
		//visualFrameTime /= 8;
		visualFrameTime = 0;
		renderVisualizer = false;
	}

	// render
	//glutPostRedisplay();

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

	//fileManager.onResize(width, height);
	TwWindowSize(width, height);
}

void mouse(int button, int state, int x, int y) {
	//fileManager.onMouse(button, state, x, y);
	TwEventMouseButtonGLUT(button, state, x, y);
}

void motion(int x, int y) {
	//fileManager.onMotion(x, y);
	TwEventMouseMotionGLUT(x, y);
}

void passiveMotion(int x, int y) {
	//fileManager.onPassiveMotion(x, y);
	TwEventMouseMotionGLUT(x, y);
}

void keyboard(unsigned char key, int x, int y) {	
	// key to change visualizer
	if (key >= '1' && key <= '5') {
		curVisualizer = key - '1';
		switchVisualizer();
	}

	// other keys
	switch (key) {
		case ' ':
			isPlaying = 1 - isPlaying;
			break;		
		case 27:
			exit(0);
			break;
	}
		
	visualizer->onKey(key);
	//fileManager.onKeyboard(key, x, y);
	TwEventKeyboardGLUT(key, x, y);
}

void special(int key, int x, int y) {
	//fileManager.onSpecial(key, x, y);
	TwEventSpecialGLUT(key, x, y);
}

// function called at exit
void termination(void)
{ 
	//fileManager.onTerminate();  
	TwTerminate();
}

void play() {
	//if (isPlaying) return;

	//isPlaying = true;
	// use Boost.Thread to create a thread for audio playback
	//boost::thread threadAudio(mainAudio);
}

/**
 Do necessary initialization to AntTweakBar library.
 */
int loadAntTweakBar() {
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
    TwDefine(" GLOBAL "); // Message added to the help bar.
    TwDefine(" Playlist size='280 440' color='128 128 128' position='200 20' valueswidth=360 fontsize=2 iconified=true"); // change default tweak bar size and color

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

void printUsage() {
	printf("--------------------------------------------------------------------------------\n");
	printf("CS5249 Audio Visualization\n");
	printf("--------------------------------------------------------------------------------\n");
	printf("[Hot keys]\n");
	printf("Space				: play/pause audio.\n");	
	printf("Left/right arrow		: change audio file.\n");
	printf("1 to 5				: change visualizer.\n");
	printf("ESC				: quit.\n");
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

