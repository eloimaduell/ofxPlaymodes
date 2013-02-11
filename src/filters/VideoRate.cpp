/*
 * VideoRate.cpp
 *
 *  Created on: 16/07/2012
 *      Author: arturo
 */

#include "filters/VideoRate.h"

namespace ofxPm{

VideoRate::VideoRate()
:source(0)
,fps(30)
,remainder(0){
	// TODO Auto-generated constructor stub

}

VideoRate::~VideoRate() {
	// TODO Auto-generated destructor stub
}


void VideoRate::setup(VideoSource & _source, float fps){
	source = &_source;
	ofAddListener(source->newFrameEvent,this,&VideoRate::newVideoFrame);
	setFps(fps);
	front = _source.getNextVideoFrame();
	//startThread(true,false);
	ofAddListener(ofEvents().update,this,&VideoRate::glThreadUpdate);
	VideoSource::width = source->getWidth();
	VideoSource::height = source->getHeight();
	
	printf("VideoRate::setup %d %d FPS %f\n",source->getWidth(),source->getHeight(),fps);
}
	
void VideoRate::removeListener()
{
	ofRemoveListener(source->newFrameEvent,this,&VideoRate::newVideoFrame);
	stopThread();
}
	

VideoFrame VideoRate::getNextVideoFrame(){
	Poco::ScopedLock<ofMutex> lock(mutexFront);
	return front;
}

void VideoRate::newVideoFrame(VideoFrame & frame){
	mutex.lock();
	back = frame;
	mutex.unlock();
}

float VideoRate::getFps(){
	return fps;
}

void VideoRate::setFps(float _fps){
	fps = _fps;
}


void VideoRate::threadedFunction(){
	while(isThreadRunning()){
		unsigned long long time = ofGetElapsedTimeMicros();
		if(back!=NULL){
			mutexFront.lock();
			framesToSend.push(back);
			mutexFront.unlock();
		}
		time = ofGetElapsedTimeMicros()-time;
		long long sleeptime =  1000000./fps-time;
		if(sleeptime>0){
			usleep(sleeptime);
		}
	}
}

void VideoRate::glThreadUpdate(ofEventArgs & args){
	double dFrames = ofGetLastFrameTime()*fps+remainder;
	int framesToSend = dFrames;
	remainder = dFrames-framesToSend;

	if(back!=NULL){
		for(int i=0;i<framesToSend;i++){
			VideoFrame newFrame = VideoFrame::newVideoFrame(back);
			ofNotifyEvent(newFrameEvent,newFrame);
		}
	}
}
	
}
