/*
 * VideoMixerGPU.h
 *
 *  Created on: Jan 18, 2013
 *      Author: arturo
 */

#ifndef VIDEOFEEDBACKPIXELSGPU_H_
#define VIDEOFEEDBACKPIXELSGPU_H_

#include "VideoFilter.h"
#include "ofxFastFboReader.h"
#include "ofPBO.h"

namespace ofxPm{
class VideoFeedbackPixelsGPU: public VideoFilter {
public:
	VideoFeedbackPixelsGPU();
	virtual ~VideoFeedbackPixelsGPU();

	void setup(VideoSource & source1, VideoSource & source2);
	void removeListener();

    VideoFrame getNextVideoFrame();
	void newVideoFrame(VideoFrame & frame);
	float getFps();
	void setFeedback(float f);
	void setInputOpacity(float f);
	
private:
	void glThreadUpdate(ofEventArgs & args);
	VideoFrame front, back, back1, back2;
	VideoSource * source1, * source2;
	float feedback;
	float inputOpacity;
	ofShader shader;
	bool newFrame;
	ofMutex swapBuffersMutex;
	
	ofFbo				fbo;
	ofPBO				pbo1,pbo2;
	ofTexture			tex1,tex2;
	ofxFastFboReader	fboReader;
	
};
}

#endif /* VIDEOFEEDBACKPIXELSGPU_H_ */
