#ifndef _GOS_RING_BUFFER_
#define _GOS_RING_BUFFER_

#include "GosMustHave.h"

#include <queue>
using namespace std;

namespace Gos
{

template <class T>
class RingBuffer : public std::deque<T>
{
public:
	RingBuffer(void) {}
	~RingBuffer(void) {}

public:
	/*
	void clear() {
		// clear the underlying container in queue
		c.clear();
	}*/
};
}

#endif