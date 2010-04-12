#include "GosGlobalVisualizer.h"

namespace Gos {
GlobalVisualizer::GlobalVisualizer(void)
{
}

GlobalVisualizer::~GlobalVisualizer(void)
{
}

void GlobalVisualizer::sleep() {
	sleepMiliseconds(1);
}

void GlobalVisualizer::update(int delta) {
	osc.update(delta);
	vis2d.update(delta);
	spec.update(delta);
}

void GlobalVisualizer::onKey(int key) {
	osc.onKey(key);
	vis2d.onKey(key);
	spec.onKey(key);
}

void GlobalVisualizer::render(Chunk& c, Rect r) {
	// divide the whole rect into three parts 
	float width = r.right - r.left;
	float height = r.top - r.bottom;
	
	float heightOsc = 0.2f * height;
	float heightSpec = heightOsc;
	float heightVis2d = height - heightOsc - heightSpec;

	Rect rectSpec(r);			
	rectSpec.bottom = 0;
	rectSpec.top	= heightSpec;
	
	Rect rectVis2d(rectSpec);	
	rectVis2d.bottom	= 0;
	rectVis2d.top		= heightVis2d;
	
	Rect rectOsc(rectVis2d);
	rectOsc.bottom  = 0;
	rectOsc.top		= heightOsc;

	spec.render(c, rectSpec);
	
	glPushMatrix();
	glTranslatef(0.0f, heightSpec, 0.0f);	
	vis2d.render(c, rectVis2d);
	
	glTranslatef(0.0f, heightVis2d, 0.0f);
	osc.render(c, rectOsc);
	glPopMatrix();
}

void GlobalVisualizer::onFileChanged(const Gos::String &file) {
	osc.onFileChanged(file);
	vis2d.onFileChanged(file);
	spec.onFileChanged(file);
}

}