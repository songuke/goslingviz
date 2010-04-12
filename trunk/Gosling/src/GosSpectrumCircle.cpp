#include "GosSpectrumCircle.h"
#include "GosChunk.h"

namespace Gos
{

SpectrumCircle::SpectrumCircle(void)
{
}

SpectrumCircle::~SpectrumCircle(void)
{
}

void SpectrumCircle::render(Chunk& c, Rect r)
{
	float width		= r.right - r.left;
	float height	= r.top - r.bottom;	
	int nbBars		= 32;
	float logBase	= 2;
	float logStep	= log((float) kChunkSize) / log(logBase);
	float centerX	= width / 2;
	float centerY	= height / 2;
	float angleStep	= M_PI / nbBars;

	Float4 lime(168.0f / 255, 230.0f / 255, 29.0f / 255, 1.0f);
	Float4 red(237.0f / 255, 28.0f / 255, 36.0f / 255, 1.0f);
	if (c.beat > 0)
	{
		lime = lime * 1.5f;
		red = red * 1.5f;
	}

	// TODO: I'll optimize the rendering with Vertex Buffer Object later.
	float angle = 0;
	int startSample;
	int endSample;
	for (int i = 0; i < nbBars; i++)
	{
		startSample = (int) floor(pow(logBase, logStep * i / nbBars)) - 1;
		endSample = (int) ceil(pow(logBase, logStep * (i + 1) / nbBars)) - 1;

		float combined = 0;
		for (int j = startSample; j < endSample; j++)
		{
			combined += pow(c.magnitude[j][1], 2);
		}
		float intensity = std::min(1.0f, combined * 0.0002f);
		Float4 curRed = lime * (1.0f - intensity) + red * intensity;
		glBegin(GL_QUADS);
			glColor4f(lime.x, lime.y, lime.z, lime.w); // bottom 
			glVertex2f(centerX,			centerY);
			glVertex2f(centerX + (intensity * sin(angle) / 2 * centerX), centerY + (intensity * cos(angle) / 2 * centerY));
			glColor4f(curRed.x, curRed.y, curRed.z, curRed.w); // top
			glVertex2f(centerX + (intensity * sin(angle + (angleStep / 2)) * centerX), centerY + (intensity * cos(angle + (angleStep / 2))) * centerY);
			glColor4f(lime.x, lime.y, lime.z, lime.w);
			glVertex2f(centerX + (intensity * sin(angle + angleStep) / 2 * centerX), centerY + (intensity * cos(angle + angleStep) / 2) * centerY);
		glEnd();

		angle += M_PI;
		combined = 0;
		for (int j = startSample; j < endSample; j++)
		{
			combined += pow(c.magnitude[j][0], 2);
		}
		intensity = std::min(1.0f, combined * 0.0002f);
		curRed = lime * (1.0f - intensity) + red * intensity;
		glBegin(GL_QUADS);
			glColor4f(lime.x, lime.y, lime.z, lime.w); // bottom 
			glVertex2f(centerX,			centerY);
			glVertex2f(centerX + (intensity * sin(angle) / 2 * centerX), centerY + (intensity * cos(angle) / 2 * centerY));
			glColor4f(curRed.x, curRed.y, curRed.z, curRed.w); // top
			glVertex2f(centerX + (intensity * sin(angle + (angleStep / 2)) * centerX), centerY + (intensity * cos(angle + (angleStep / 2))) * centerY);
			glColor4f(lime.x, lime.y, lime.z, lime.w);
			glVertex2f(centerX + (intensity * sin(angle + angleStep) / 2 * centerX), centerY + (intensity * cos(angle + angleStep) / 2) * centerY);
		glEnd();

		angle -= M_PI;
		angle += angleStep;
	}
}

}