#ifndef _GOS_LINE_
#define _GOS_LINE_

#include "GosMustHave.h"

namespace Gos
{
class Line
{
public:
	Line(void);
	~Line(void);

public:
	static void draw(Image* image, Float2 src, Float2 dest, Float4 color);

	/**
	Return additional mask marking the drawn pixels.
	*/
	static void draw(Image* image, Float2 src, Float2 dest, Float4 color, Image* mask);
};
}

#endif