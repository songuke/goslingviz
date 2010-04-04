#ifndef _GOS_WARP_
#define _GOS_WARP_

#include "GosMustHave.h"

namespace Gos
{

class Warp
{
public:
	Warp(void);
	~Warp(void);

public:
	void update(int delta);
	void render(Image* imageIn, Image* imageOut, Chunk &c);
protected:
	inline Float2 Warp::warp(Float2 p) {
		Float2 q;
		float v = 4.0f;
		Float2 n = p.normalize();
		q = p - n * v;
		return q;
	}
};
}

#endif