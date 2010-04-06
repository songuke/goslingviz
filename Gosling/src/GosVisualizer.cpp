#include "GosVisualizer.h"

namespace Gos
{
Visualizer::Visualizer(void)
: timeElapsed(0)
{
}

Visualizer::~Visualizer(void)
{
}

void Visualizer::update(int delta) {
	timeElapsed += delta;
}

}

