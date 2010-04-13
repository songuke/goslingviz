#ifndef _GOS_UTIL_
#define _GOS_UTIL_

#include <stdlib.h>
#include <math.h>
#include <boost/thread.hpp>
#include <stdio.h>
#include <stdarg.h>

namespace Gos
{
	#define RENZODERER_API 

	#define mmax(a, b) a > b ? a : b
	#define mmin(a, b) a < b ? a : b

	#define safeFree(x) if(x) { free(x); x = 0; }
	#define safeDel(x) if(x) { delete x; x = 0; }
	#define safeDelArray(x) if(x) { delete [] x; x = 0; }

	// constant
	const double from255to1 = 0.003921568627450980392156862745098;

	// integer swap
	#define SWAP(x, y) (x ^= y ^= x ^= y)

	// image
	struct Size {
		int width;
		int height;
	};
	
	enum PixelFormat {
		rzRGB8,
		rzRGBA8,
		rzRGB16,
		rzRGBA16,
		rzRGB32,
		rzRGBA32
	};

	extern "C" {
		void  rzInfo(const char*, ...);
		void  rzWarning(const char*, ...);
		void  rzError(const char*, ...);
		void  rzSevere(int exitCode, const char*, ...);

		RENZODERER_API inline void  rzSevere(int exitCode, const char* str, ...) {
			String msg;
			msg.append("SEVERE: ");
			msg.append(str);

			va_list ap;
			va_start(ap, str);
			//Log::instance()->write(msg.c_str(), ap);
			vfprintf(stderr, msg.c_str(), ap);
			va_end(ap);
			
			exit(exitCode);
		}

		/* rzAssert */
		#define EXIT_ASSERT 10
		#ifdef NDEBUG
		#define rzAssert(expr) ((void)0)
		#else
		#define rzAssert(expr) \
		((expr) ? (void)0 : rzSevere(EXIT_ASSERT, "Assertion " #expr " failed in %s, line %d", __FILE__, __LINE__))
		#endif	  
	};

	inline double random(double _min, double _max) {		
		//double val = (1.0 * rand() / RAND_MAX) * (_max - _min) + _min;
		//printf("%f\n", val);
		//return val;
		return (1.0 * rand() / RAND_MAX) * (_max - _min) + _min;
	}

	inline int randomInteger(int _min, int _max) {
		return rand() % (_max - _min + 1) + _min;
	}

	inline double log2(double x) {
		return log(x) / log(2.0);
	}
	
	inline void sleepMiliseconds(int milis) {
		boost::system_time time = boost::get_system_time();
		time += boost::posix_time::millisec(milis);
		boost::thread::sleep(time); 
	}
}

#endif