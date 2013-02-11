/*
 * AudioFrame.h
 *
 *  Created on: 09-oct-2008
 *      Author: arturo castro
 */

#include "VideoGrabber.h"

namespace ofxPm{
VideoGrabber::VideoGrabber(){
}

VideoGrabber::~VideoGrabber(){
}

bool VideoGrabber::initGrabber(int w, int h){
	bool ret = ofVideoGrabber::initGrabber(w,h,false);
	frame    = VideoFrame::newVideoFrame(getPixelsRef());
	VideoSource::width    = ofVideoGrabber::getWidth();
	VideoSource::height   = ofVideoGrabber::getHeight();
	
	printf("VideoGrabber::initGrabber %d %d\n",VideoSource::width,VideoSource::height);
	return ret;
}

VideoFrame VideoGrabber::getNextVideoFrame(){
    return frame;
}

void VideoGrabber::update(){
	ofVideoGrabber::update();
	if(isFrameNew()){
		newFrame(getPixelsRef());
	}
}

void VideoGrabber::newFrame(ofPixels & pixels){
	frame = VideoFrame::newVideoFrame(pixels);
	//frame.getTextureRef();
	newFrameEvent.notify(this,frame);
}

//------------------------------------------------------
float VideoGrabber::getFps(){
	return fps;
}
	
//------------------------------------------------------
void VideoGrabber::setFps(float fps){
	this->fps = fps;
}
	
	;
	
}
