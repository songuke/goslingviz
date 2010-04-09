#ifndef _GOS_IMAGE_
#define _GOS_IMAGE_

#include "GosMustHave.h"

namespace Gos
{
class Image
{
public:
	/**
	Create empty image buffer with specified input size.
	*/
	Image(int width, int height);
	Image(int width, int height, int channels);

	/**
	Image from a given buffer.
	*/
	Image(byte* buffer, int width, int height, int channels);

	~Image(void);
public:
	String	getName() const;
	byte*	getPixels() const;
	int		getWidth()	const;
	int		getHeight() const;
	int		getChannels() const;

	inline void Image::setPixel(const Float2& p, const Float4& color) {
		// check out of bound
		if (isOutOfBound(p)) return;

		int index = (int)p.y * width + (int)p.x;
		for (int i = 0; i < channels; ++i)
			buffer[channels * index + i] = 255 * color[i];
	}

	inline float Image::clampX(float x) {
		if (x < 0) {
			x = 0;
			//p.y = floor(p.y / stepI) * stepI;
		}
		else if (x >= width) {
			x = (int)((width - 1) / stepJ) * stepJ;
			//p.y = floor(p.y / stepI) * stepI;
		}
		return x;		
	}

	inline float Image::clampY(float y) {
		if (y < 0) {
			y = 0;
		}
		else if (y >= height) {
			y = (int)((height - 1) / stepI) * stepI;
		}
		return y;
	}

	inline Float2 Image::clamp(Float2 p) {
		p.x = clampX(p.x);
		p.y = clampY(p.y);
		return p;
	}

	/**
	Get pixel value at the input location.
	If the location is not on a pixel, perform bilinear interpolation
	and return the interpolated color.
	*/
	inline Float4 Image::getPixelBilinear(const Float2& p) {
		int x0 = floor(p.x / stepJ) * stepJ;
		int x1 = ceil(p.x / stepJ) * stepJ;
		int y0 = floor(p.y / stepI) * stepI;
		int y1 = ceil(p.y / stepI) * stepI;
	
		x0 = clampX(x0);
		x1 = clampX(x1);
		y0 = clampY(y0);
		y1 = clampY(y1);

		// D --- C
		// |  E  |
		// A --- B		
		float s = (p.x - x0) / stepJ;
		float t = (p.y - y0) / stepI;
		
		// interpolate on x
		Float4 colorA = getPixel(Float2(x0, y0));
		Float4 colorB = getPixel(Float2(x1, y0));
		Float4 colorC = getPixel(Float2(x1, y1));
		Float4 colorD = getPixel(Float2(x0, y1));
		Float4 colorAB = colorA * (1 - s) + colorB * s;
		Float4 colorDC = colorD * (1 - s) + colorC * s;
		
		// interpolate on y
		return colorAB * (1 - t) + colorDC * t;

	}

	inline Float4 Image::getPixel(const Float2& position) {
		Float4 color;
		Float2 p(position);
		
		int index = (int)p.y * width + (int)p.x;
		for (int i = 0; i < channels; ++i) 
			color[i] = buffer[channels * index + i] / 255.0f;
		color.w = 1.0f;
		return color;
	}

	/**
	Get the pixel value at the input location.
	Non-integer location are truncated to integer.
	*/
	/*
	inline Float4 Image::getPixel(const Float2& position) {
		Float4 color;
		Float2 p(position);
		//if (isOutOfBound(p)) // return color;
		//{
		
		if (p.x < 0) p.x = 0;
		else if (p.x >= width) p.x = width - 1;
		if (p.y < 0) p.y = 0;
		else if (p.y >= height) p.y = height - 1;
			//p.x = mmax(mmin(p.x, width - 1.0f), 0.0f);
			//p.y = mmax(mmin(p.y, height - 1.0f), 0.0f);
		//}

		int index = (int)p.y * width + (int)p.x;
		for (int i = 0; i < channels; ++i) 
			color[i] = buffer[channels * index + i] / 255.0f;
		color.w = 1.0f;
		return color;
	}*/
	
	/**
	Transform the coordinate in the center origin 
	to the lower left origin.
	Input position should be in normalized coordinates [-1, 1]
	*/
	inline Float2 Image::transformLowerLeft(const Float2& position) {
		//return position + center;
		Float2 q;
		q.x = (position.x + 1) * width * 0.5f;
		q.y = (position.y + 1) * height * 0.5f;
		return q;
	}

	/**
	Transform the coordinate in the lower left origin
	to the center origin.
	Also normalize the coordinate to [-1, 1].
	*/
	inline Float2 Image::transformCenter(const Float2& position) {
		//return position - center;
		Float2 q;
		q.x = 2 * position.x / width - 1;
		q.y = 2 * position.y / height - 1;
		return q;
	}

	/**
	Return the center coordinates of the image.
	*/
	inline Float2 Image::getCenter() const {
		return center;
	}

	/**
	Make a copy of current image.
	*/
	Image*	clone();
	
	/**
	Copy the content of the input image.
	*/
	void	copy(Image* im);

	/**
	Check if the input image is the same size and channels with current image.
	*/
	bool	isSameSize(Image* im);

	/**
	Interpolate remaining pixels after sample values are set at grid points.
	*/
	void	setGrid(int stepI, int stepJ);
	void	interpolateAtGrid();

	/**
	Copy pixels from the input image and scale it to current image size.
	*/
	void	scaleFrom(Image* im);

	/**
	Clear the image to 0.
	*/
	void	clear();
protected:
	inline bool Image::isOutOfBound(const Float2& p) {
		if (p.x < 0 || p.x >= width || p.y < 0 || p.y >= height)
			return true;
		return false;
	}

protected:
	String name;
	byte* buffer;
	int width, height, channels;
	int stepI, stepJ;
	float* s, *t;			// interpolated coefficients in a grid cell
	Float2 center;

	Image* tmp;
};
}

#endif