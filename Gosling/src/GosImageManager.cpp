#include "GosImageManager.h"
#include "GosImage.h"

#include <FreeImage/FreeImage.h>

namespace Gos
{
static ImageManager* __imageManager = 0;

ImageManager* ImageManager::instance() {
	if (__imageManager == 0) {
		__imageManager = new ImageManager;
	}
	return __imageManager;
}

ImageManager::ImageManager() {	
	// call this ONLY when linking with FreeImage as a static library
	#ifdef FREEIMAGE_LIB
		FreeImage_Initialise();
	#endif
}

ImageManager::~ImageManager() {
	// empty the images array
	for (ImageList::iterator i = images.begin(); i != images.end(); i++) {		
		safeDel(i->second);
	}
	images.clear();

	// call this ONLY when linking with FreeImage as a static library
	#ifdef FREEIMAGE_LIB
		FreeImage_DeInitialise();
	#endif

	safeDel(__imageManager);
}

Image* ImageManager::load(const String& file) {
	ImageList::iterator i = images.find(file);
	if (i != images.end()) { // this image is already loaded 
		return i->second;
	} else {

		void* pixels;
		PixelFormat fmt;
		Size size;
		this->loadInternal(file, pixels, fmt, size);

		// TODO: only support RGB8 at the moment
		Image* img = new Image((byte*)pixels, size.width, size.height, 3);
		//Image* img = (Image*)Renzoderer::instance()->getResourceManager()->createObject(ImageType);
		//img->initialize(pixels, fmt, size, file);

		// house keeping in imageList
		images[file] = img;
		return img;
	}
}

/*
void ImageManager::unload(Image *img) {
	ImageList::iterator i = images.find(img->getName());
	if (i != images.end()) { // this image is loaded and it is safe to unload
		images.erase(i);
	}
	safeDel(img);	
}*/

/**
 FreeImage library loads the image up-side-down. The image needs to be flipped so as to be used correctly in OpenGL.
 */
void ImageManager::loadInternal(const String& file, void*& pixels, PixelFormat& fmt, Size& size) {
	const char* filename = file.c_str();
	pixels = 0;

	//image format
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	//pointer to the image, once loaded
	FIBITMAP *dib(0);
	//pointer to the image data
	//BYTE* bits(0);
	//image width and height
	unsigned int width(0), height(0);
	//OpenGL's image ID to map to
	//GLuint gl_texID;
	
	//check the file signature and deduce its format
	fif = FreeImage_GetFileType(filename, 0);
	//if still unknown, try to guess the file format from the file extension
	if(fif == FIF_UNKNOWN) 
		fif = FreeImage_GetFIFFromFilename(filename);
	//if still unkown, return failure
	if(fif == FIF_UNKNOWN)
		return;

	//check that the plugin has reading capabilities and load the file
	if(FreeImage_FIFSupportsReading(fif))
		dib = FreeImage_Load(fif, filename);
	//if the image failed to load, return failure
	if(!dib)
		return;

	//retrieve the image data
	//pixels = FreeImage_GetBits(dib);
	//get the image width and height
	size.width = FreeImage_GetWidth(dib);
	size.height = FreeImage_GetHeight(dib);
	
	// determine image format
	int bpp = FreeImage_GetBPP(dib);

	// number of input channels
	int nbComponents = bpp / 8;

	// number of output channels
	int nbChannels;	// cannot determine channels by bpp / 8, as some images have palette inside (e.g., PCX)
	
	// get image type
	FREE_IMAGE_TYPE imageType = FreeImage_GetImageType(dib);
	// get color type
	FREE_IMAGE_COLOR_TYPE colorType = FreeImage_GetColorType(dib);
	switch (colorType) {
		case FIC_MINISBLACK:
			nbChannels = 1;
			break;
		case FIC_PALETTE:
			nbChannels = 3;
			break;
		case FIC_RGB:
			nbChannels = 3;
			break;
		case FIC_RGBALPHA:
			nbChannels = 4;
			break;
	}

	// allocate output buffer
	unsigned char* _pixels = (unsigned char*) malloc (size.width * size.height * nbChannels);
	pixels = _pixels;

	// if this somehow one of these failed (they shouldn't), return failure
	if((pixels == 0) || (size.width == 0) || (size.height == 0))
		return;
	
	int nbPixels = 0;
	switch (colorType) {
		case FIC_PALETTE: 
			{
			// if the bitmap is palette-based, we need to decode the color
			int paletteSize = FreeImage_GetColorsUsed(dib);
			if (paletteSize > 0) // palette size == 0 means no palette so do nothing
			{
				RGBQUAD* pal = FreeImage_GetPalette(dib);
				//for (int i = 0; i < 256; ++i) 
					//printf("%d %d %d\n", pal[i].rgbRed, pal[i].rgbGreen, pal[i].rgbBlue);

				fmt = rzRGB8;
				for( int y = 0; y < size.height; y++ )
				{
					BYTE *dibData = FreeImage_GetScanLine( dib, size.height - y - 1 );

					for( int x = 0; x < size.width; x++) 
					{
						
						_pixels[nbPixels++] = pal[*dibData].rgbRed;
						_pixels[nbPixels++] = pal[*dibData].rgbGreen;
						_pixels[nbPixels++] = pal[*dibData].rgbBlue;
						/*
						_pixels[nbPixels++] = *dibData;
						_pixels[nbPixels++] = *dibData;
						_pixels[nbPixels++] = *dibData;
						*/
						dibData += nbComponents;
					}
				}
			} else {
				// TODO: load normal 8-bit bitmap 

			}
			break;
			}
		
		case FIC_RGB:
			{
			fmt = rzRGB8;
			for( int y = 0; y < size.height; y++ )
			{
				BYTE *dibData = FreeImage_GetScanLine( dib, size.height - y - 1 );

				for( int x = 0; x < size.width; x++) 
				{
					_pixels[nbPixels++] = dibData[FI_RGBA_RED];
					_pixels[nbPixels++] = dibData[FI_RGBA_GREEN];
					_pixels[nbPixels++] = dibData[FI_RGBA_BLUE];
					dibData += nbComponents;
				}
			}
			break;
			}
		case FIC_RGBALPHA:
			fmt = rzRGBA8;
			break;
	}
	
	/*
	printf("Debug load image\n");
	for (int i = 0; i < size.height; ++i) {
		unsigned char* data = _pixels + i * 3 * size.width;
		for (int j = 0; j < size.width; ++j) {
			printf("%d ", data[0]);
			printf("%d ", data[1]);
			printf("%d ", data[2]);
			data += 3;
		}
		printf("\n");
	}*/
	

	//if this texture ID is in use, unload the current texture
	//if(m_texID.find(texID) != m_texID.end())
		//glDeleteTextures(1, &(m_texID[texID]));

	//generate an OpenGL texture ID for this texture
	//glGenTextures(1, &gl_texID);
	//store the texture ID mapping
	//m_texID[texID] = gl_texID;
	//bind to the new texture ID
	//glBindTexture(GL_TEXTURE_2D, gl_texID);
	//store the texture data for OpenGL use
	//glTexImage2D(GL_TEXTURE_2D, level, internal_format, width, height,
		//border, image_format, GL_UNSIGNED_BYTE, bits);

	//Free FreeImage's copy of the data
	FreeImage_Unload(dib);
}

}