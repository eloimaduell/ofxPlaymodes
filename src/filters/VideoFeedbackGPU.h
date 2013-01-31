/*
 * VideoMixerGPU.h
 *
 *  Created on: Jan 18, 2013
 *      Author: arturo
 */

#ifndef VIDEOFEEDBACKGPU_H_
#define VIDEOFEEDBACKGPU_H_

#include "VideoFilter.h"

namespace ofxPm{
class VideoFeedbackGPU: public VideoFilter {
public:
	VideoFeedbackGPU();
	virtual ~VideoFeedbackGPU();

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
};
}

#endif /* VIDEOMIXERGPU_H_ */
