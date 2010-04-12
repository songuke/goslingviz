#include "GosAudioOut.h"
#include "GosChunk.h"

namespace Gos
{
AudioOut::AudioOut(void)
: wvOut(0), frames(0), channels(0)
{
}

AudioOut::~AudioOut(void)
{
	safeDel(wvOut);
	safeDel(frames);
}

void AudioOut::setChannels(int channels) {
	if (this->channels != channels) {
		this->channels = channels;
		// reset wvOut
		safeDel(wvOut);
		wvOut = new RtWvOut(channels, kSampleRate, 0, kOutputBufferSize, 32);

		// buffer
		safeDel(frames);
		frames = new StkFrames(kChunkSize, channels);
	}
}

void AudioOut::tick(double sample) {
	wvOut->tick(sample);
}

void AudioOut::tickChunk(Chunk& c) {
	for (int i = 0; i < kChunkSize; ++i) {
		for (int j = 0; j < channels; ++j) {
			(*frames)[i * channels + j] = c.amplitude[i][j];
		}
	}
	wvOut->tick(*frames);
}

void AudioOut::stop() {
	wvOut->stop();
}

void AudioOut::onFileChanged(const String& file) {
	this->stop();
}

}

