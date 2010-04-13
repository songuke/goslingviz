#include "GosSpectrogram.h"
#include "GosChunk.h"

namespace Gos
{

Spectrogram::Spectrogram(void)
: needRender(false), coolFactor(0), coolVel(0), coolAccel(0)
{
	lastChunk	= new Chunk;
	nbBars		= 32;	
	coolFactor	= new float[nbBars];
	coolVel		= new float[nbBars];
	coolAccel	= new float[nbBars];	

	logBase	= 2;
	logStep	= log((float) kChunkSize) / log(logBase);
}

Spectrogram::~Spectrogram(void)
{
	safeDel(lastChunk);	
	safeDel(coolFactor);
	safeDel(coolVel);
	safeDel(coolAccel);
}

void Spectrogram::renderChunk(Chunk& c, Rect r)
{
	float width		= r.right - r.left;
	float height	= r.top - r.bottom;	
	float gapSize	= width / nbBars * 0.08;
	float barWidth  = width / nbBars * 0.92;
	float barHeight	= height;

	// update bar height up to cool factor
	for (int i = 0; i < nbBars; ++i) {		
		coolFactor[i]	+= coolVel[i];
		coolVel[i]		+= coolAccel[i];
		if (coolFactor[i] < 0) 
			coolFactor[i] = 0;
		if (coolFactor[i] > 1) {
			coolFactor[i] = 1;
			coolVel[i] = -coolVel[i] * 0.5f;
			coolAccel[i] = 0;
		}
	}

	// if every coolFactor is 0, draw the next chunk
	bool isZero = true;
	for (int i = 0; i < nbBars; ++i) {
		if (coolFactor[i] != 0) {
			isZero = false;
			break;
		}
	}
	if (isZero)
		needRender = true;

	Float4 lime(168.0f / 255, 230.0f / 255, 29.0f / 255, 1.0f);
	Float4 red(237.0f / 255, 28.0f / 255, 36.0f / 255, 1.0f);

	// TODO: I'll optimize the rendering with Vertex Buffer Object later.
	float x = r.left + gapSize;
	float y = r.bottom;
	int startSample;
	int endSample;
	for (int i = 0; i < nbBars; i++)
	{
		startSample = (int) floor(pow(logBase, logStep * i / nbBars)) - 1;
		endSample = (int) ceil(pow(logBase, logStep * (i + 1) / nbBars)) - 1;
		float combined = 0;
		for (int j = startSample; j < endSample; j++)
		{
			combined += pow(fabs(c.magnitude[j][0] + c.magnitude[j][1]), 2);
		}
		float intensity = std::min(1.0f, combined * 0.0001f);

		intensity *= coolFactor[i];

		Float4 curRed = lime * (1.0f - intensity) + red * intensity;
		glBegin(GL_QUADS);
			glColor4f(lime.x, lime.y, lime.z, lime.w); // bottom 
			glVertex2f(x,				y);
			glVertex2f(x + barWidth,	y);
			glColor4f(curRed.x, curRed.y, curRed.z, curRed.w); // top
			glVertex2f(x + barWidth,	y + barHeight * intensity);
			glVertex2f(x,				y + barHeight * intensity);
		glEnd();

		x += barWidth + gapSize;
	}
}

void Spectrogram::render(Chunk& c, Rect r) {
	// wait until next beat
	if (needRender) {
		memcpy(lastChunk, &c, sizeof(Chunk));		
		needRender = false;
		// grow up
		int startSample;
		int endSample;
		for (int i = 0; i < nbBars; ++i) {
			startSample = (int) floor(pow(logBase, logStep * i / nbBars)) - 1;
			endSample = (int) ceil(pow(logBase, logStep * (i + 1) / nbBars)) - 1;
			float combined = 0;
			for (int j = startSample; j < endSample; j++)
			{
				combined += pow(fabs(c.magnitude[j][0] + c.magnitude[j][1]), 2);
			}
			float intensity = std::min(1.0f, combined * 0.0001f);

			coolVel[i] = 0.3f * intensity;
			coolAccel[i] = 0.1f;
		}
	}
	renderChunk(*lastChunk, r);
}

void Spectrogram::onFileChanged(const Gos::String &file) {
}

void Spectrogram::onBeat() {
	needRender = true;
}

}