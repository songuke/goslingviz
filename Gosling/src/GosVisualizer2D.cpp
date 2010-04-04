#include "GosVisualizer2D.h"

#include "GosChunk.h"

namespace Gos
{
Visualizer2D::Visualizer2D(void)
: buffer(0)
{
}

Visualizer2D::~Visualizer2D(void)
{
	safeFree(buffer);
}

void Visualizer2D::setBufferSize(int w, int h) {
	// initialize a new buffer with current size
	if (buffer == 0) {
		buffer = new Image(w, h, 3);
		//width = w; height = h;
		//buffer = (byte*) malloc(sizeof(byte) * 3 * width * height);
		//memset(buffer, 0, sizeof(byte) * 3 * width * height);
	} else {
		//if (this->width != w || this->height != h) { // size changed
		if (buffer->getWidth() != w || buffer->getHeight() != h) { // size changed
			safeDel(buffer);
			buffer = new Image(w, h, 3);

			//width = w; height = h;
			//safeFree(buffer);
			//buffer = (byte*) malloc(sizeof(byte) * 3 * width * height);
			//memset(buffer, 0, sizeof(byte) * 3 * width * height);
		}
	}
}

void Visualizer2D::render(Chunk& c, Rect r) {
	// prepare render buffer
	float width		= r.right - r.left;
	float height	= r.top - r.bottom;	
	this->setBufferSize((int)width, (int)height);
	if (buffer == 0) {
		throw std::exception("Render buffer null.\n");
	}

	// render to buffer
	this->renderBuffer(c, r);

	// render buffer to screen using OpenGL glDrawPixels function
	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	glDisable(GL_LIGHTING);
	glRasterPos2f(0, 0);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE, buffer->getPixels());
	glPopAttrib();
}
}
