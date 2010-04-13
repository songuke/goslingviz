// GosMustHave.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#ifndef _GOS_MUST_HAVE_
#define _GOS_MUST_HAVE_

#define _USE_MATH_DEFINES
#include "targetver.h"

#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <math.h>
#include <string>
//#include <windows.h> // min, max -> use std::min std::max instead

#include <errno.h>
#include <iostream>
#include <boost/exception/all.hpp>

//FFTW library
#include <fftw3/fftw3.h>

/**
 OpenGL utility library
 */
#if defined(_MACOSX)
#   include <GLUT/glut.h>
#else
#   include <GL/glut.h>
#endif

namespace Gos
{
//-----------------------------------------------------------------------------
// Data structure
//-----------------------------------------------------------------------------
	typedef unsigned char byte;
	typedef std::string String;
	template<class T> class RingBuffer;
	
//-----------------------------------------------------------------------------
// Constant
//-----------------------------------------------------------------------------
	const int kSampleRate		= 44100;
	const int kOutputBufferSize = 1024;
	const int kNbBuffers		= 64;

//-----------------------------------------------------------------------------
// Guitar model
//-----------------------------------------------------------------------------
	class Parameter {};
	class PluckedStringParameter;
	class GuitarKeyboardParameter;
		
	class Excitation;
	class PluckedString;
	class GuitarBody;
	class GuitarKeyboard;
	class GuitarPlayer;

//-----------------------------------------------------------------------------
// Visualization
//-----------------------------------------------------------------------------
	struct Chunk;
	const int kChunkSize = 1024;
	const int nbChannels = 2;

	struct Rect { float left, top, right, bottom; };
	class Image;
	class ImageManager;

	class Line;
	class Curve;
	
	class Visualizer;
	class Visualizer2D;
	class Oscilloscope;
	class Spectrogram;
	class SpectrumCircle;
	class GlobalVisualizer;
	class CurveWarp;

	class BeatDetector;

	class FileChangedHandler {
	public:
		virtual void onFileChanged(const String& file) = 0;
	};

//-----------------------------------------------------------------------------
// Boost definition
//-----------------------------------------------------------------------------
	typedef boost::error_info<struct tag_errno, int> ErrnoInfo; 
	typedef boost::error_info<struct tag_file_name, std::string> FileNameInfo;

	class FileReadError : public boost::exception {};
}

#include "GosUtil.h"
#include "GosFloat2.h"
#include "GosFloat4.h"

//------------------------------------------------------------------------------
// Util functions that cannot be put into GosUtil appears here
//------------------------------------------------------------------------------
namespace Gos {
	// color from HSV to RGB
	/* value: [0, 1]
	 * hue: [0, 360], in float for higher resolution
	 * saturation [0, 1]
	 */
	Float4 hsv2rgb(float& value, float& hue, float& saturation);
}

#endif
