/*
 * VideoMixerGPU.h
 *
 *  Created on: Jan 18, 2013
 *      Author: arturo
 */

#ifndef VIDEODRYWETPIXELSGPU_H_
#define VIDEODRYWETPIXELSGPU_H_

#include "VideoFilter.h"
#include "ofxFastFboReader.h"
#include "ofPBO.h"


namespace ofxPm{
class VideoDryWetPixelsGPU: public VideoFilter {
public:
	VideoDryWetPixelsGPU();
	virtual ~VideoDryWetPixelsGPU();

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
	
	ofFbo				fbo;
	ofPBO				pbo1,pbo2;
	ofTexture			tex1,tex2;
	ofxFastFboReader	fboReader;	
	
};
}

#endif 
