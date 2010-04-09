#include "GosImage.h"

namespace Gos
{
Image::Image(int _width, int _height) 
: width(_width), height(_height), channels(1),
  s(0), t(0)
{
	buffer = (byte*) malloc(width * height * channels * sizeof(byte));
	memset(buffer, 0, width * height * channels * sizeof(byte));
	center.x = floor(width * 0.5f);
	center.y = floor(height * 0.5f);
	this->setGrid(1, 1);
}

Image::Image(int _width, int _height, int _channels) 
: width(_width), height(_height), channels(_channels),
  s(0), t(0)
{
	buffer = (byte*) malloc(width * height * channels * sizeof(byte));
	memset(buffer, 0, width * height * channels * sizeof(byte));
	center.x = floor(width * 0.5f);
	center.y = floor(height * 0.5f);
	this->setGrid(1, 1);
}

Image::Image(byte* _buffer, int _width, int _height, int _channels)
: buffer(_buffer), width(_width), height(_height), channels(_channels),
  s(0), t(0)
{
	center.x = floor(width * 0.5f);
	center.y = floor(height * 0.5f);
	this->setGrid(1, 1);
}

Image::~Image(void)
{
	safeFree(buffer);
	safeDelArray(s);
	safeDelArray(t);
}

int Image::getChannels() const {
	return channels;
}

int Image::getWidth() const {
	return width;
}

int Image::getHeight() const {
	return height;
}

byte* Image::getPixels() const {
	return buffer;
}

String Image::getName() const {
	return name;
}

Image* Image::clone() {
	Image* im = new Image(width, height, channels);
	im->center = center;
	im->setGrid(stepI, stepJ);
	memcpy(im->getPixels(), this->getPixels(), sizeof(byte) * width * height * channels);
	return im;
}

bool Image::isSameSize(Image* im) {
	return (width == im->getWidth() && height == im->getHeight() && channels == im->getChannels());
}

void Image::copy(Image* im) {
	if (! isSameSize(im))
		throw std::exception("Copy image size not equal.");

	im->center = center;
	im->setGrid(stepI, stepJ);
	memcpy(this->getPixels(), im->getPixels(), sizeof(byte) * width * height * channels);
}

void Image::setGrid(int _stepI, int _stepJ) {
	if (stepI == _stepI && stepJ == _stepJ) return;

	this->stepI = _stepI;
	this->stepJ = _stepJ;

	// precompute interpolated factors
	int cellSize = stepI * stepJ;
	safeDelArray(s);
	safeDelArray(t);
	s = new float[cellSize];
	t = new float[cellSize];
	float fStepI = 1.0f / stepI;
	float fStepJ = 1.0f / stepJ;
	float curStepI = 0.0f;
	float curStepJ = 0.0f;
	for (int i = 0; i < stepI; ++i) { 
		curStepJ = 0.0f;
		for (int j = 0; j < stepJ; ++j) {
			s[i * stepJ + j] = curStepJ;
			t[i * stepJ + j] = curStepI;

			curStepJ += fStepJ;
		}
		curStepI += fStepI;
	}
}

/**
Two pass bilinear interpolation
*/
void Image::interpolateAtGrid() {	
	Float4 colorA;
	Float4 colorB;
	Float4 colorC;
	Float4 colorD;
	Float4 colorAB;
	Float4 colorDC;
	Float4 color;

	int x0, x1;

	int y0 = 0;
	int y1 = y0;

	for (int i = 0; i < height; i += stepI) {
		// reset at every row
		x0 = 0;
		x1 = x0 + stepJ;
		if (x1 >= width) x1 = x0;

		// increment at every row
		y1 += stepI;
		if (y1 >= height) y1 = y0;

		colorA = getPixel(Float2(x0, y0));
		colorB = getPixel(Float2(x1, y0));
		colorC = getPixel(Float2(x1, y1));
		colorD = getPixel(Float2(x0, y1));
		for (int j = 0; j < width; j += stepJ) {
			// D --- C
			// |  E  |
			// A --- B
			for (int y = 0; y < stepI && i + y < height; ++y) {
				for (int x = 0; x < stepJ && j + x < width; ++x) {
					if (x == 0 && y == 0) continue;
					colorAB = colorA * (1 - s[y * stepJ + x]) + colorB * s[y * stepJ + x];
					colorDC = colorD * (1 - s[y * stepJ + x]) + colorC * s[y * stepJ + x];
					color	= colorAB * (1 - t[y * stepJ + x]) + colorDC * t[y * stepJ + x];
					this->setPixel(Float2(j + x, i + y), color);
				}
			}

			// interpolate next cell
			x0 = x1;
			x1 += stepJ;
			if (x1 >= width) x1 = x0;
			
			colorA = colorB;
			colorD = colorC;
			colorB = getPixel(Float2(x1, y0));
			colorC = getPixel(Float2(x1, y1));
		}

		y0 = y1;
	}
}
/*
void Image::interpolateAtGrid() {
	Float4 colorA;
	Float4 colorB;
	Float4 colorC;
	Float4 colorD;
	Float4 colorAB;
	Float4 colorDC;
	Float4 color;

	int x0, x1;

	int y0 = 0;
	int y1 = y0;

	for (int i = 0; i < height; i += stepI) {
		// reset at every row
		x0 = 0;
		x1 = x0 + stepJ;
		if (x1 >= width) x1 = x0;

		// increment at every row
		y1 += stepI;
		if (y1 >= height) y1 = y0;

		colorA = getPixel(Float2(x0, y0));
		colorB = getPixel(Float2(x1, y0));
		colorC = getPixel(Float2(x1, y1));
		colorD = getPixel(Float2(x0, y1));
		for (int j = 0; j < width; j += stepJ) {
			// D --- C
			// |  E  |
			// A --- B			
			for (int y = 0; y < stepI && i + y < height; ++y) {
				for (int x = 0; x < stepJ && j + x < width; ++x) {
					if (x == 0 && y == 0) continue;
					colorAB = colorA * (1 - s[y * stepJ + x]) + colorB * s[y * stepJ + x];
					colorDC = colorD * (1 - s[y * stepJ + x]) + colorC * s[y * stepJ + x];
					color	= colorAB * (1 - t[y * stepJ + x]) + colorDC * t[y * stepJ + x];
					this->setPixel(Float2(j + x, i + y), color);
				}
			}

			// interpolate next cell
			x0 = x1;
			x1 += stepJ;
			if (x1 >= width) x1 = x0;
			
			colorA = colorB;
			colorD = colorC;
			colorB = getPixel(Float2(x1, y0));
			colorC = getPixel(Float2(x1, y1));
		}

		y0 = y1;
	}
}*/

void Image::scaleFrom(Image* im) {
	float stepWidth = (float) im->getWidth() / width;
	float stepHeight = (float) im->getHeight() / height;
	Float2 s(stepWidth, stepHeight);
	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			Float2 p(j, i);
			Float2 q = p * s;
			this->setPixel(p, im->getPixelBilinear(q));
		}
	}
}

void Image::clear() {
	memset(buffer, 0, width * height * channels * sizeof(byte));
}

}
