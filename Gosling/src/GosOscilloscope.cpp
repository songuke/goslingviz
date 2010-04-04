#include "GosOscilloscope.h"
#include "GosChunk.h"

namespace Gos
{
Oscilloscope::Oscilloscope(void)
{
}

Oscilloscope::~Oscilloscope(void)
{
}

void Oscilloscope::render(Chunk& c, Rect r) {
	// simplest implementation of oscilloscope	
	float width		= r.right - r.left;
	float height	= r.top - r.bottom;	
	float gapSize	= 4.0f;	
	float barWidth  = 48.0f;
	int nbBars		= floor((width - gapSize) / (barWidth + gapSize));
	int sampleStep	= kChunkSize / nbBars;
	float barHeight	= height;

	Float4 lime(168.0f / 255, 230.0f / 255, 29.0f / 255, 1.0f);
	Float4 red(237.0f / 255, 28.0f / 255, 36.0f / 255, 1.0f);

	// TODO: I'll optimize the rendering with Vertex Buffer Object later.
	float x = r.left + gapSize;
	float y = r.bottom;
	for (int i = 0; i < kChunkSize; i += sampleStep) {
		float intensity = std::min(1.0f, (float)(1.8f * (fabs(c.amplitude[i][0]) + fabs(c.amplitude[i][1])) * 0.5f)); // blow it up a bit
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

}