//-----------------------------------------------------------------------------
// OpenGL visualization for guitar synthesized sound.
// Control UI by AntTweakBar UI library.
//
// Son Hua, NUS
// 2010-03-09
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// This code adapts from AntTweakBar's sample using GLUT. The mainAudio()
// function is called by a seperate thread to synthetize audio and send to
// output. The main thread is used for rendering (do nothing at the moment
// except the TweakBar.
//-----------------------------------------------------------------------------

/**
 My guitar synthesis library
 */
#include "GosMustHave.h"
#include "GosPluckedString.h"
#include "GosGuitarKeyboard.h"
#include "GosGuitarPlayer.h"
using namespace Gos;

/**
 STK library for audio playback and output to file.
 */
#include "RtWvOut.h"
#include "FileWvOut.h"
using namespace stk;

/**
 AntTweakBar library to make a simple UI for parameter tweak.
 */
#include <AntTweakBar/AntTweakBar.h>

/**
 OpenGL utility library for waveform display
 */
#if defined(_MACOSX)
#   include <GLUT/glut.h>
#else
#   include <GL/glut.h>
#endif

/**
 Boost.Thread library
 */
#include <boost/thread.hpp>
//-----------------------------------------------------------------------------
// Audio
//-----------------------------------------------------------------------------
GuitarKeyboard*				keyboard;
GuitarKeyboardParameter*	keyboardParam;
GuitarPlayer*				player;

// adjustable parameter
float			paramGainFractional			= 0.5;
float			paramAmplitude				= 1.0;
unsigned int	paramMinimumTimePlay		= 5000;		
double			paramThresRemove			= 10;
//double			paramThresLowSound			= 1.0e-5;
double			paramThresSilent			= 100;		// number of silent frames

// buffer setting
unsigned int bufferSize		= 1024;
unsigned int nbBuffers		= 32;
float sampleRate			= 44100;


/**
 Debug
 */
void onFreqChanged(double* freqs, int n) {	
	printf("Now playing frequencies: ");
	for (int j = 0; j < n; ++j) {
		printf("%.2f\t", freqs[j]);
	}
	printf("\n");
}

void updateParams() {
	keyboardParam->string.amplitude = paramAmplitude;
	keyboardParam->string.gain = (99 + paramGainFractional) * 0.01;
	keyboardParam->minimumTimePlay = paramMinimumTimePlay;
	keyboardParam->thresRemove = paramThresRemove * 1.0e-7;
	keyboardParam->thresLowSound = keyboardParam->thresRemove * 10;
	keyboardParam->thresSilent = paramThresSilent;
	keyboard->setGuitarKeyboardParameter(keyboardParam);
}

/**
 * Send audio samples to output in a separate thread. 
 */
void mainAudio() {
	printf("Audio playback thread started.\n");

	String title = "ff9theme";
	//String title = "ff8_eyes_on_me";
	//String title = "ff7_aerith";
	//String title = "mary_had_a_little_lamb";

	String tabFile = "tablature/";
	tabFile.append(title);
	tabFile.append(".txt");

	String wavFile = "output/";
	wavFile.append(title);
	wavFile.append(".wav");

	keyboard			= new GuitarKeyboard;
	keyboardParam		= new GuitarKeyboardParameter;
	player				= new GuitarPlayer;
	//keyboard->onFrequenciesChanged(onFreqChanged); // register a callback
	updateParams();	// must init params for keyboard (some internal variables of keyboard depends on these params at start).
	
	try {
		player->loadTablatureText(tabFile);
	} catch (boost::exception& e) {
		cerr << diagnostic_information(e);
		
		int * err = boost::get_error_info<ErrnoInfo>(e);
		exit(*err);
	}
	Stk::setSampleRate(sampleRate);
	Stk::showWarnings( true );

	RtWvOut* dac = 0;
	FileWvOut* file = 0;
	
	try {
		// Define and open the default realtime output device for one-channel playback
		// Default buffer size is RT_BUFFER_SIZE and is set to 512. I saw some clicks in my output, so buffer size is set bigger.
		dac = new RtWvOut( 1, sampleRate, 0, bufferSize, 32 );
		file = new FileWvOut(wavFile);
	}
	catch ( StkError & ) {
		exit( 1 );
	}

	while (player->hasNext()) {
		String keys = player->sample();
		
		keyboard->pluck(keys);		
							
		while (keyboard->isLowSound() == 0) {
			// update guitar parameters
			updateParams();

			keyboard->tic();
			double sample = keyboard->sample();
			
		
			dac->tick(sample);
			file->tick(sample);
		}
		
		player->tic();
	}
	file->closeFile();
	delete dac;
	delete file;
	delete keyboard;
	delete keyboardParam;
	delete player;

	printf("Audio playback completed.\n");
}

//-----------------------------------------------------------------------------
// Rendering
//-----------------------------------------------------------------------------
// This example displays one of the following shapes
typedef enum { SHAPE_TEAPOT=1, SHAPE_TORUS, SHAPE_CONE } Shape;
#define NUM_SHAPES 3
Shape g_CurrentShape = SHAPE_TORUS;
// Shapes scale
float g_Zoom = 1.0f;
// Shape orientation (stored as a quaternion)
float g_Rotation[] = { 0.0f, 0.0f, 0.0f, 1.0f };
// Auto rotate
int g_AutoRotate = 0;
int g_RotateTime = 0;
float g_RotateStart[] = { 0.0f, 0.0f, 0.0f, 1.0f };
// Shapes material
float g_MatAmbient[] = { 0.5f, 0.0f, 0.0f, 1.0f };
float g_MatDiffuse[] = { 1.0f, 1.0f, 0.0f, 1.0f };
// Light parameter
float g_LightMultiplier = 1.0f;
float g_LightDirection[] = { -0.57735f, -0.57735f, -0.57735f };

// Routine to set a quaternion from a rotation axis and angle
// ( input axis = float[3] angle = float  output: quat = float[4] )
void SetQuaternionFromAxisAngle(const float *axis, float angle, float *quat)
{
    float sina2, norm;
    sina2 = (float)sin(0.5f * angle);
    norm = (float)sqrt(axis[0]*axis[0] + axis[1]*axis[1] + axis[2]*axis[2]);
    quat[0] = sina2 * axis[0] / norm;
    quat[1] = sina2 * axis[1] / norm;
    quat[2] = sina2 * axis[2] / norm;
    quat[3] = (float)cos(0.5f * angle);

}


// Routine to convert a quaternion to a 4x4 matrix
// ( input: quat = float[4]  output: mat = float[4*4] )
void ConvertQuaternionToMatrix(const float *quat, float *mat)
{
    float yy2 = 2.0f * quat[1] * quat[1];
    float xy2 = 2.0f * quat[0] * quat[1];
    float xz2 = 2.0f * quat[0] * quat[2];
    float yz2 = 2.0f * quat[1] * quat[2];
    float zz2 = 2.0f * quat[2] * quat[2];
    float wz2 = 2.0f * quat[3] * quat[2];
    float wy2 = 2.0f * quat[3] * quat[1];
    float wx2 = 2.0f * quat[3] * quat[0];
    float xx2 = 2.0f * quat[0] * quat[0];
    mat[0*4+0] = - yy2 - zz2 + 1.0f;
    mat[0*4+1] = xy2 + wz2;
    mat[0*4+2] = xz2 - wy2;
    mat[0*4+3] = 0;
    mat[1*4+0] = xy2 - wz2;
    mat[1*4+1] = - xx2 - zz2 + 1.0f;
    mat[1*4+2] = yz2 + wx2;
    mat[1*4+3] = 0;
    mat[2*4+0] = xz2 + wy2;
    mat[2*4+1] = yz2 - wx2;
    mat[2*4+2] = - xx2 - yy2 + 1.0f;
    mat[2*4+3] = 0;
    mat[3*4+0] = mat[3*4+1] = mat[3*4+2] = 0;
    mat[3*4+3] = 1;
}


// Routine to multiply 2 quaternions (ie, compose rotations)
// ( input q1 = float[4] q2 = float[4]  output: qout = float[4] )
void MultiplyQuaternions(const float *q1, const float *q2, float *qout)
{
    float qr[4];
	qr[0] = q1[3]*q2[0] + q1[0]*q2[3] + q1[1]*q2[2] - q1[2]*q2[1];
	qr[1] = q1[3]*q2[1] + q1[1]*q2[3] + q1[2]*q2[0] - q1[0]*q2[2];
	qr[2] = q1[3]*q2[2] + q1[2]*q2[3] + q1[0]*q2[1] - q1[1]*q2[0];
	qr[3]  = q1[3]*q2[3] - (q1[0]*q2[0] + q1[1]*q2[1] + q1[2]*q2[2]);
    qout[0] = qr[0]; qout[1] = qr[1]; qout[2] = qr[2]; qout[3] = qr[3];
}


// Callback function called by GLUT to render screen
void Display(void)
{
    float v[4]; // will be used to set light paramters
    float mat[4*4]; // rotation matrix

    // Clear frame buffer
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/*
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glEnable(GL_NORMALIZE);

    // Set light
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    v[0] = v[1] = v[2] = g_LightMultiplier*0.4f; v[3] = 1.0f;
    glLightfv(GL_LIGHT0, GL_AMBIENT, v);
    v[0] = v[1] = v[2] = g_LightMultiplier*0.8f; v[3] = 1.0f;
    glLightfv(GL_LIGHT0, GL_DIFFUSE, v);
    v[0] = -g_LightDirection[0]; v[1] = -g_LightDirection[1]; v[2] = -g_LightDirection[2]; v[3] = 0.0f;
    glLightfv(GL_LIGHT0, GL_POSITION, v);

    // Set material
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, g_MatAmbient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, g_MatDiffuse);

    // Rotate and draw shape
    glPushMatrix();
    glTranslatef(0.5f, -0.3f, 0.0f);
    if( g_AutoRotate ) 
    {
        float axis[3] = { 0, 1, 0 };
        float angle = (float)(glutGet(GLUT_ELAPSED_TIME)-g_RotateTime)/1000.0f;
        float quat[4];
        SetQuaternionFromAxisAngle(axis, angle, quat);
        MultiplyQuaternions(g_RotateStart, quat, g_Rotation);
    }
    ConvertQuaternionToMatrix(g_Rotation, mat);
    glMultMatrixf(mat);
    glScalef(g_Zoom, g_Zoom, g_Zoom);
    glCallList(g_CurrentShape);
    glPopMatrix();
	*/

    // Draw tweak bars
    TwDraw();

    // Present frame buffer
    glutSwapBuffers();

    // Recall Display at next frame
    glutPostRedisplay();
}


// Callback function called by GLUT when window size changes
void Reshape(int width, int height)
{
    // Set OpenGL viewport and camera
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(40, (double)width/height, 1, 10);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0,0,5, 0,0,0, 0,1,0);
    glTranslatef(0, 0.6f, -1);

    // Send the new window size to AntTweakBar
    TwWindowSize(width, height);
}


// Function called at exit
void Terminate(void)
{ 
    glDeleteLists(SHAPE_TEAPOT, NUM_SHAPES);

    TwTerminate();
}


//  Callback function called when the 'AutoRotate' variable value of the tweak bar has changed
void TW_CALL SetAutoRotateCB(const void *value, void *clientData)
{
    (void)clientData; // unused

    g_AutoRotate = *(const int *)(value); // copy value to g_AutoRotate
    if( g_AutoRotate!=0 ) 
    {
        // init rotation
        g_RotateTime = glutGet(GLUT_ELAPSED_TIME);
        g_RotateStart[0] = g_Rotation[0];
        g_RotateStart[1] = g_Rotation[1];
        g_RotateStart[2] = g_Rotation[2];
        g_RotateStart[3] = g_Rotation[3];

        // make Rotation variable read-only
        TwDefine(" TweakBar/ObjRotation readonly ");
    }
    else
        // make Rotation variable read-write
        TwDefine(" TweakBar/ObjRotation readwrite ");
}


//  Callback function called by the tweak bar to get the 'AutoRotate' value
void TW_CALL GetAutoRotateCB(void *value, void *clientData)
{
    (void)clientData; // unused
    *(int *)(value) = g_AutoRotate; // copy g_AutoRotate to value
}

void printUsage() {
	printf("--------------------------------------------------------------------------------\n");
	printf("CS5249 Guitar Sound Synthesis\n");
	printf("Hua Binh Son, HT081432W\n");
	printf("huabinhs@comp.nus.edu.sg\n");
	printf("--------------------------------------------------------------------------------\n");
}
 
// Main
int main(int argc, char *argv[])
{
    TwBar *bar; // Pointer to the tweak bar
    float axis[] = { 0.7f, 0.7f, 0.0f }; // initial model rotation
    float angle = 0.8f;

    // Initialize AntTweakBar
    // (note that AntTweakBar could also be intialized after GLUT, no matter)
    if( !TwInit(TW_OPENGL, NULL) )
    {
        // A fatal error occured    
        fprintf(stderr, "AntTweakBar initialization failed: %s\n", TwGetLastError());
        return 1;
    }

    // Initialize GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(320, 480);
    glutCreateWindow("CS5249 Guitar Sound Synthesis");
    glutCreateMenu(NULL);

    // Set GLUT callbacks
    glutDisplayFunc(Display);
    glutReshapeFunc(Reshape);
    atexit(Terminate);  // Called after glutMainLoop ends

    // Set GLUT event callbacks
    // - Directly redirect GLUT mouse button events to AntTweakBar
    glutMouseFunc((GLUTmousebuttonfun)TwEventMouseButtonGLUT);
    // - Directly redirect GLUT mouse motion events to AntTweakBar
    glutMotionFunc((GLUTmousemotionfun)TwEventMouseMotionGLUT);
    // - Directly redirect GLUT mouse "passive" motion events to AntTweakBar (same as MouseMotion)
    glutPassiveMotionFunc((GLUTmousemotionfun)TwEventMouseMotionGLUT);
    // - Directly redirect GLUT key events to AntTweakBar
    glutKeyboardFunc((GLUTkeyboardfun)TwEventKeyboardGLUT);
    // - Directly redirect GLUT special key events to AntTweakBar
    glutSpecialFunc((GLUTspecialfun)TwEventSpecialGLUT);
    // - Send 'glutGetModifers' function pointer to AntTweakBar;
    //   required because the GLUT key event functions do not report key modifiers states.
    TwGLUTModifiersFunc(glutGetModifiers);

    // Create some 3D objects (stored in display lists)
	/*
    glNewList(SHAPE_TEAPOT, GL_COMPILE);
    glutSolidTeapot(1.0);
    glEndList();
    glNewList(SHAPE_TORUS, GL_COMPILE);
    glutSolidTorus(0.3, 1.0, 16, 32);
    glEndList();
    glNewList(SHAPE_CONE, GL_COMPILE);
    glutSolidCone(1.0, 1.5, 64, 4);
    glEndList();
	*/

    // Create a tweak bar
    bar = TwNewBar("TweakBar");
    TwDefine(" GLOBAL help='Sound synthesis parameters.' "); // Message added to the help bar.
    TwDefine(" TweakBar size='280 440' color='128 128 128' "); // change default tweak bar size and color

	// gain
	TwAddVarRW(bar, "Gain", TW_TYPE_FLOAT, &paramGainFractional, 
               " min=0.0 max=0.60 step=0.01 keyIncr=g keyDecr=G help='Gain fractional value. Gain value = (99 + gain fractional value) / 100. Should be between 0.0 and 0.60.' ");
	// max amplitude
	TwAddVarRW(bar, "Amplitude", TW_TYPE_FLOAT, &paramAmplitude, 
               " min=0.1 max=1.0 step=0.1 keyIncr=a keyDecr=A help='Amplitude value. Should be between 0.1 and 1.0.' ");
	
	// minimum time play
	TwAddVarRW(bar, "Minium Time Play", TW_TYPE_UINT32, &paramMinimumTimePlay, 
               " min=1000 max=50000 step=1000 keyIncr=t keyDecr=T help='Minimum time (in frames unit) a note must play.' ");
	
	// thresRemove
	TwAddVarRW(bar, "Removal Threshold", TW_TYPE_DOUBLE, &paramThresRemove, 
               " min=1 max=100 step=1.0 keyIncr=r keyDecr=R help='Threshold to stop playing a note when its amplitude is too low.' ");

	// silent frames
	TwAddVarRW(bar, "Silent Length", TW_TYPE_DOUBLE, &paramThresSilent, 
               " min=0 max=10000 step=100 keyIncr=s keyDecr=S help='Number of silent frames between tabs.' ");

	// audio device list

	/*
    // Add 'g_Zoom' to 'bar': this is a modifable (RW) variable of type TW_TYPE_FLOAT. Its key shortcuts are [z] and [Z].
    TwAddVarRW(bar, "Zoom", TW_TYPE_FLOAT, &g_Zoom, 
               " min=0.01 max=2.5 step=0.01 keyIncr=z keyDecr=Z help='Scale the object (1=original size).' ");

    // Add 'g_Rotation' to 'bar': this is a variable of type TW_TYPE_QUAT4F which defines the object's orientation
    TwAddVarRW(bar, "ObjRotation", TW_TYPE_QUAT4F, &g_Rotation, 
               " label='Object rotation' open help='Change the object orientation.' ");

    // Add callback to toggle auto-rotate mode (callback functions are defined above).
    TwAddVarCB(bar, "AutoRotate", TW_TYPE_BOOL32, SetAutoRotateCB, GetAutoRotateCB, NULL, 
               " label='Auto-rotate' key=space help='Toggle auto-rotate mode.' ");

    // Add 'g_LightMultiplier' to 'bar': this is a variable of type TW_TYPE_FLOAT. Its key shortcuts are [+] and [-].
    TwAddVarRW(bar, "Multiplier", TW_TYPE_FLOAT, &g_LightMultiplier, 
               " label='Light booster' min=0.1 max=4 step=0.02 keyIncr='+' keyDecr='-' help='Increase/decrease the light power.' ");

    // Add 'g_LightDirection' to 'bar': this is a variable of type TW_TYPE_DIR3F which defines the light direction
    TwAddVarRW(bar, "LightDir", TW_TYPE_DIR3F, &g_LightDirection, 
               " label='Light direction' open help='Change the light direction.' ");

    // Add 'g_MatAmbient' to 'bar': this is a variable of type TW_TYPE_COLOR3F (3 floats color, alpha is ignored)
    // and is inserted into a group named 'Material'.
    TwAddVarRW(bar, "Ambient", TW_TYPE_COLOR3F, &g_MatAmbient, " group='Material' ");

    // Add 'g_MatDiffuse' to 'bar': this is a variable of type TW_TYPE_COLOR3F (3 floats color, alpha is ignored)
    // and is inserted into group 'Material'.
    TwAddVarRW(bar, "Diffuse", TW_TYPE_COLOR3F, &g_MatDiffuse, " group='Material' ");

    // Add the enum variable 'g_CurrentShape' to 'bar'
    // (before adding an enum variable, its enum type must be declared to AntTweakBar as follow)
    {
        // ShapeEV associates Shape enum values with labels that will be displayed instead of enum values
        TwEnumVal shapeEV[NUM_SHAPES] = { {SHAPE_TEAPOT, "Teapot"}, {SHAPE_TORUS, "Torus"}, {SHAPE_CONE, "Cone"} };
        // Create a type for the enum shapeEV
        TwType shapeType = TwDefineEnum("ShapeType", shapeEV, NUM_SHAPES);
        // add 'g_CurrentShape' to 'bar': this is a variable of type ShapeType. Its key shortcuts are [<] and [>].
        TwAddVarRW(bar, "Shape", shapeType, &g_CurrentShape, " keyIncr='<' keyDecr='>' help='Change object shape.' ");
    }
	*/

	/*
    // Store time
    g_RotateTime = glutGet(GLUT_ELAPSED_TIME);
    // Init rotation
    SetQuaternionFromAxisAngle(axis, angle, g_Rotation);
    SetQuaternionFromAxisAngle(axis, angle, g_RotateStart);
	*/

	// use Boost.Thread to create a thread for audio playback
	boost::thread threadAudio(mainAudio);

    // Call the GLUT main loop
    glutMainLoop();

    return 0;
}

