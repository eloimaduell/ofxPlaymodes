/*
 * VideoMixerGPU.h
 *
 *  Created on: Jan 18, 2013
 *      Author: arturo
 */

#ifndef VIDEODRYWETGPU_H_
#define VIDEODRYWETGPU_H_

#include "VideoFilter.h"

namespace ofxPm{
class VideoDryWetGPU: public VideoFilter {
public:
	VideoDryWetGPU();
	virtual ~VideoDryWetGPU();

	void setup(VideoSource & source1, VideoSource & source2);
	void removeListener();

    VideoFrame getNextVideoFrame();
	void newVideoFrame(VideoFrame & frame);
	float getFps();
	void setDryWet(float f);
	
private:
	void glThreadUpdate(ofEventArgs & args);
	VideoFrame front, back, back1, back2;
	VideoSource * source1, * source2;
	float dryWetA;
	float dryWetB;
	ofShader shader;
	bool newFrame;
	ofMutex swapBuffersMutex;
};
}

#endif 
