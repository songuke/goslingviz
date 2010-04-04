#ifndef _GOS_VISUALIZER_2D_
#define _GOS_VISUALIZER_2D_

#include "GosMustHave.h"
#include "GosVisualizer.h"
#include "GosImage.h"

namespace Gos
{
/**
2D effects for visualization. Everything is drawn to an internal buffer.
The buffer is then drawn to window.
*/
class Visualizer2D : public Visualizer
{
public:
	Visualizer2D(void);
	virtual ~Visualizer2D(void);

public:
	/**
	Set buffer size. Buffer size should be larger than the rendered rect.
	Usually buffer size is set to the area of visualization.
	*/
	void setBufferSize(int width, int height); 
	
	/**
	Render to buffer then draw buffer to screen.
	*/
	void render(Chunk& c, Rect r);

protected:
	/**
	Render everything into buffer.
	*/
	virtual void renderBuffer(Chunk& c, Rect r) = 0;

protected:
	Image* buffer;
};
}

#endif