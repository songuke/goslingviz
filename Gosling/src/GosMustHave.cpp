// GosMustHave.cpp : source file that includes just the standard includes
// Gosling.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "GosMustHave.h"

// TODO: reference any additional headers you need in GOSMUSTHAVE.H
// and not in this file

namespace Gos {
	// color from HSV to RGB
	/* value: [0, 1]
	 * hue: [0, 360], in float for higher resolution
	 * saturation [0, 1]
	 */
	Float4 hsv2rgb(float& value, float& hue, float& saturation) {
		int h;
		float f, p, q, t, section;
		float r, g, b;
		
		if (saturation == 0) {//gray
			r = g = b = value;
		} else {
			section = hue / 60.f;
			h = (int)section;
			f = section - h;
			p = value * (1 - saturation);
			q = value * (1 - f*saturation);
			t = value * (1 - (1 - f)*saturation);
			
			switch (h) {
			case 0:
				r = value;
				g = t;
				b = p;
				break;
			case 1:
				r = q;
				g = value;
				b = p;
				break;
			case 2:
				r = p;
				g = value;
				b = t;
				break;
			case 3:
				r = p;
				g = q;
				b = value;
				break;
			case 4:
				r = t;
				g = p;
				b = value;
				break;
			case 5:
				r = value;
				g = p;
				b = q;
				break;
			}
		}

		return Float4(r, g, b, 1.0f);		
	}
}
