#include "GosAudioIn.h"
#include "GosChunk.h"
#include "GosVisualizer.h"

#include <cmath>

namespace Gos {
AudioIn::AudioIn(void)
: wvIn(0), next(0), frames(0)
{
	
}

AudioIn::~AudioIn(void)
{
	safeDel(wvIn);
	safeDel(frames);
	if (plan != NULL)
		fftw_destroy_plan(plan);
	fftw_free(in);
	fftw_free(out);
}

void AudioIn::loadAudio(const String& file) {
	try
	{
		safeDel(wvIn);
		bool normalized = true; // ask for normalization to [-1, 1]
		wvIn = new FileWvIn(file, false, normalized, 1000000, kChunkSize);
		wvIn->setRate(wvIn->getFileRate() / kSampleRate);
		wvIn->normalize(1.0);

		safeDel(frames);
		frames = new StkFrames(kChunkSize, wvIn->channelsOut());
		
		// fftw allocation
		in	= (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * kChunkSize);
        out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * kChunkSize);
		plan = fftw_plan_dft_1d(kChunkSize, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
		if (plan == NULL)
			printf("Null plan!\n");

		// reset
		next = 0;		
	} catch (boost::exception& x) {
		FileReadError e;
		e << ErrnoInfo(errno) << FileNameInfo(file);
		throw e;
	}
}

void AudioIn::tick() {
	if (wvIn)
		wvIn->tick();
}

double AudioIn::sample() {
	return wvIn->lastOut();
}

void AudioIn::sampleChunk(Chunk& c) {
	//
	// time domain
	//
	//memset(c.amplitude, 0, kChunkSize * nbChannels * sizeof(double));
	memset(c.amplitude, 0, sizeof(c.amplitude));

	wvIn->tick(*frames);
	if (frames->size() > 0) {
		for (int i = 0; i < wvIn->getSize(); ++i) {
			for (int j = 0; j < wvIn->channelsOut(); ++j) {
				double sample = (*frames)[i * wvIn->channelsOut() + j];
				c.amplitude[i][j] = sample; // already normalized
			}
		}
	}
	// TODO: need overlapping chunks return. They are used for render, but not for audio output

	//
	// frequency domain
	//
	// TODO: use FFTW to transform the chunk to frequency domain. Need overlap chunk.
	for (int i = 0; i < nbChannels; ++i)
	{
		for (int j = 0; j < kChunkSize; ++j)
		{
			in[j][0] = c.amplitude[j][i] * hamming(j, kChunkSize);
		}

		// run fft for each channel
		fftw_execute(plan);

		// copy results to chunk
		for (int j = 0; j < kChunkSize; ++j)
		{
			c.magnitude[j][i]	= fabs(	out[j][0]	);
			c.phase[j][i]		=		out[j][1];
		}
	}

	c.beat = beatDetector.hasBeat(c);
	if (c.beat == 2)
	{
		// notify all observers
		for (int i = 0; i < observerVis.size(); ++i) {
			observerVis[i]->onBeat();
		}
	}
}

bool AudioIn::hasNext() const {
	return !wvIn->isFinished();
}

int AudioIn::getChannels() const {
	return wvIn->channelsOut();
}

float AudioIn::hamming(int n, int bigN)
{
	return 0.54 - 0.46 * cos(2 * PI * n / (bigN - 1));
}

void AudioIn::observeBeatFor(Visualizer* vis) {
	observerVis.push_back(vis);
}

}

