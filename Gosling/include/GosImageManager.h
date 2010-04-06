#ifndef _GOS_IMAGE_MANAGER_
#define _GOS_IMAGE_MANAGER_

#include "GosMustHave.h"

namespace Gos
{
class ImageManager
{
private:
	ImageManager(void);
public:
	virtual ~ImageManager(void);

public:
	static ImageManager* instance();
	Image* load(const String& file);	
	//void unload(Image* img);
protected:
	virtual void loadInternal(const String& file, void*& pixels, PixelFormat& fmt, Size& size);
protected:
	typedef std::map<String, Image*> ImageList;
	ImageList images;
};
}

#endif
