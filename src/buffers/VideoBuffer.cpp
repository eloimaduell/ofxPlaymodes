/*
 * VideoBuffer.cpp
 *
 *  Created on: 09-oct-2008
 *      Author: arturo castro
 */

#include "VideoBuffer.h"

namespace ofxPm{
VideoBuffer::VideoBuffer(VideoSource & source, int size) {
	setup(source,size);
}

VideoBuffer::VideoBuffer(){
	source = NULL;
	totalFrames=0;
	stopped = false;
	maxSize = 0;
	microsOneSec=0;
	realFps = 0;
	framesOneSec = 0;
	feedBackIn=0.0f;
	pixelsIn.allocate(640,480,OF_PIXELS_RGB);	

}


void VideoBuffer::setup(VideoSource & source, int size){
	this->source=&source;
	totalFrames=0;
	maxSize = size;
	resume();
	microsOneSec=ofGetElapsedTimeMicros();
	feedBackIn=0.0f;
	pixelsIn.allocate(640,480,OF_PIXELS_RGB);	


}

VideoBuffer::~VideoBuffer() {

}

void VideoBuffer::newVideoFrame(VideoFrame & frame){
	unsigned long time = frame.getTimestamp().epochMicroseconds();
	framesOneSec++;
	if(time-microsOneSec>=1000000){
		realFps = framesOneSec;
		framesOneSec = 0;
		microsOneSec = time;
	}
    totalFrames++;
    if(size()==0)initTime=frame.getTimestamp();
	
    
	//-- feedback shit

	if(feedBackIn>0.0f) 
	{
		VideoFrame tempVideoFrame;
		tempVideoFrame = sourceIn->getNextVideoFrame();
		ofColor color;
		ofColor colGrab;
		ofColor colSrc;
		
		for(int x=0;x<(640);x++)
		{
			for(int y=0;y<(480);y++)
			{
				// ??clamp doesn't work ?¿
				colGrab = ofColor(frame.getPixelsRef().getColor(x,y));
				colSrc = ofColor(tempVideoFrame.getPixelsRef().getColor(x,y))*feedBackIn;
				color = colGrab + colSrc;
				pixelsIn.setColor(x,y,color);
				
				 
				
				/* 
				int red = frame.getPixelsRef().getColor(x,y)[0];
				int green = frame.getPixelsRef().getColor(x,y)[1];
				int blue = frame.getPixelsRef().getColor(x,y)[2];
				
				int red2 = tempVideoFrame.getPixelsRef().getColor(x,y)[0];
				int green2 = tempVideoFrame.getPixelsRef().getColor(x,y)[1];
				int blue2 = tempVideoFrame.getPixelsRef().getColor(x,y)[2];
								
				if(int(red) + int(float(red2*feedBackIn)) > 255) color[0] = 255;
				else color[0] = int(red) + int(float(red2*feedBackIn));
				
				if(int(green) + int(float(green2*feedBackIn)) > 255) color[1] = 255;
				else color[1] = int(green) + int(float(green2*feedBackIn));

				if(int(blue) + int(float(blue2*feedBackIn)) > 255) color[2] = 255;
				else color[2] = int(blue) + int(float(blue2*feedBackIn));
				
				 pixelsIn.setColor(x,y,color);
				 
				 */
				
				
			}
		}
	}
	else pixelsIn = frame.getPixelsRef();
	
	
	VideoFrame frameWithFeedback = VideoFrame::newVideoFrame(pixelsIn);
	timeMutex.lock();
	frames.push_back(frameWithFeedback);	
	//frames.push_back(frame);
    while(size()>maxSize){
        frames.erase(frames.begin());
    }
    timeMutex.unlock();
    newFrameEvent.notify(this,frameWithFeedback);

}

Timestamp VideoBuffer::getLastTimestamp(){
    if(size()>0)
        return frames.back().getTimestamp();
    else
        return Timestamp();
}

TimeDiff VideoBuffer::getTotalTime(){
    return getLastTimestamp()-getInitTime();
}

Timestamp VideoBuffer::getInitTime(){
    return initTime;
}

unsigned int VideoBuffer::size(){
    return frames.size();
}


unsigned int VideoBuffer::getMaxSize(){
	return maxSize;
}


float VideoBuffer::getFps(){
    return source->getFps();
}

VideoFrame VideoBuffer::getVideoFrame(TimeDiff time){
    VideoFrame frame;
    if(size()>0){
        int frameback = CLAMP((int)((float)time/1000000.0*(float)getFps()),1,size());
        int currentPos = CLAMP(size()-frameback,0,size()-1);
        frame = frames[currentPos];
    }

    return frame;

}

VideoFrame VideoBuffer::getVideoFrame(int position){
    //return buffer.find(times[times.size()-position])->second;
    if(size()){
        position = CLAMP(position,0,size()-1);
        //cout << "frame " << position << " retained " << frames[position]->_useCountOfThisObject << "\n";
        return frames[position];
    }else{
        return VideoFrame();
    }
}

VideoFrame VideoBuffer::getVideoFrame(float pct){
    return getVideoFrame(getLastTimestamp()-(getInitTime()+getTotalTime()*pct));
}

VideoFrame VideoBuffer::getNextVideoFrame(){
    return getVideoFrame((int)size()-1);
}

long VideoBuffer::getTotalFrames(){
    return totalFrames;
}

float VideoBuffer::getRealFPS(){
    return realFps;
}


void VideoBuffer::draw(){
		
    float length = (float(size())/float(maxSize))*(ofGetWidth()-(PMDRAWSPACING*2));
    float oneLength=float(ofGetWidth()-PMDRAWSPACING*2)/(float)(maxSize);
	int sizeInOneLengths= oneLength*size();
	int drawBufferY = ofGetHeight() -80;
	int originXAtEnd = ofGetWidth() - PMDRAWSPACING;

	ofEnableAlphaBlending();
	
	ofSetColor(25,25,25,240);
	ofRect(PMDRAWSPACING,ofGetHeight()-140,ofGetWidth()-2*PMDRAWSPACING,100);
	
    if(!stopped) ofSetColor(255,0,0);
	else ofSetColor(255);
	//ofSetColor(255);
	ofLine(originXAtEnd-int(sizeInOneLengths),drawBufferY,originXAtEnd,drawBufferY);
	
	
	ofSetColor(255);
	
	int fps = getFps();
    char measureMessage[10];
    for(int i=0;i<size()+1;i=i+5){
		
       if((size()-i)%fps==0){
		   ofSetColor(200);
		   ofRect(originXAtEnd-(oneLength*(i)),drawBufferY,oneLength,-10);
		   ofLine(originXAtEnd-(oneLength*(i)),drawBufferY,originXAtEnd-(oneLength*(i)),drawBufferY-10);
		   ofDrawBitmapString(ofToString(int((float(size())-float(i))/float(fps)))+"s",originXAtEnd-(oneLength*(i)),drawBufferY+15);
//            ofLine(originXAtEnd - (oneLength*i),710,originXAtEnd - (oneLength*i),700);
//            sprintf(measureMessage,"%0.2f",(float)(frames[i]->getTimestamp()-initTime)/1000000.0);
//            ofDrawBitmapString(measureMessage,originXAtEnd - (oneLength*i),695);
        }
	   else {
		   ofSetColor(64);
		   ofLine(originXAtEnd-(oneLength*(i)),drawBufferY,originXAtEnd-(oneLength*(i)),drawBufferY-10);
	   }
    }
	ofDisableAlphaBlending();
	ofSetColor(255);

}


void VideoBuffer::stop(){
	ofRemoveListener(source->newFrameEvent,this,&VideoBuffer::newVideoFrame);
    stopped = true;
	
}

void VideoBuffer::resume(){
	ofAddListener(source->newFrameEvent,this,&VideoBuffer::newVideoFrame);
    stopped = false;
}

bool VideoBuffer::isStopped(){
	return stopped;
}

void VideoBuffer::clear(){
    while(!frames.empty()){
        frames.erase(frames.begin());
    }
}

void VideoBuffer::setInSource(VideoSource & s)
{
	this->sourceIn = &s;
}
	
void VideoBuffer::setFeedBack(float f)
{
	feedBackIn = f;
}

}

