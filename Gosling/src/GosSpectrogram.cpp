#include "GosSpectrogram.h"
#include "GosChunk.h"

namespace Gos
{

Spectrogram::Spectrogram(void)
{
}

Spectrogram::~Spectrogram(void)
{
}

void Spectrogram::render(Chunk& c, Rect r)
{
	float width		= r.right - r.left;
	float height	= r.top - r.bottom;	
	int nbBars		= 32;
	float logBase	= 2;
	float logStep	= log((float) kChunkSize) / log(logBase);
	float gapSize	= width / nbBars * 0.08;
	float barWidth  = width / nbBars * 0.92;
	float barHeight	= height;

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

void Spectrogram::onFileChanged(const Gos::String &file) {
}

}