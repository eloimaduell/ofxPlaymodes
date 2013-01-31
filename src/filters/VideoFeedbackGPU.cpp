/*
 * VideoFeedbackGPU.cpp
 *
 *  Created on: Jan 18, 2013
 *      Author: arturo
 */

#include "VideoFeedbackGPU.h"
using Poco::ScopedLock;

#define STRINGIFY(...) #__VA_ARGS__

static string fragmentFeedbackSrc =
#ifdef TARGET_LINUX
		"#version 140\n"
		"#extension GL_ARB_texture_rectangle : enable\n"
#endif
        STRINGIFY(
		uniform sampler2DRect tex0;
        uniform sampler2DRect tex1;
		uniform float feedback;		  
		uniform float opacityIn;		  
        void main (void){
            vec2 pos = gl_FragCoord.xy;

            vec4 color = vec4(max(texture2DRect(tex0, pos).r* opacityIn,texture2DRect(tex1, pos).r*(feedback)),max(texture2DRect(tex0, pos).g*opacityIn,texture2DRect(tex1, pos).g*(feedback)),max(texture2DRect(tex0, pos).b*opacityIn,texture2DRect(tex1, pos).b*(feedback)),1.0);

            gl_FragColor = color;
        }
        );

namespace ofxPm{
VideoFeedbackGPU::VideoFeedbackGPU()
:source1(0)
,source2(0)
,newFrame(false)
{
	// TODO Auto-generated constructor stub

}

VideoFeedbackGPU::~VideoFeedbackGPU() {
	// TODO Auto-generated destructor stub
}

void VideoFeedbackGPU::setup(VideoSource & _source1, VideoSource & _source2){
	source1 = &_source1;
	source2 = &_source2;
	front = VideoFrame::newVideoFrame(_source1.getNextVideoFrame());
	back = VideoFrame::newVideoFrame(_source1.getNextVideoFrame());
	back.setTextureOnly(true);
	ofAddListener(source1->newFrameEvent,this,&VideoFeedbackGPU::newVideoFrame);
	shader.unload();
	shader.setupShaderFromSource(GL_FRAGMENT_SHADER,fragmentFeedbackSrc);
	shader.linkProgram();

}

void VideoFeedbackGPU::removeListener()
{
	ofRemoveListener(source1->newFrameEvent,this,&VideoFeedbackGPU::newVideoFrame);
	//stopThread();
}
	
	
VideoFrame VideoFeedbackGPU::getNextVideoFrame(){
	return front;
}

void VideoFeedbackGPU::newVideoFrame(VideoFrame & frame){
	//front = VideoFrame::newVideoFrame(frame);

	if(source2->getNextVideoFrame()==NULL){
		ofNotifyEvent(newFrameEvent,front);
		return;
	}


	back.getFboRef().begin();
	shader.begin();
	shader.setUniformTexture("tex0",frame.getTextureRef(),0);
	shader.setUniformTexture("tex1",source2->getNextVideoFrame().getTextureRef(),1);
	shader.setUniform1f("feedback",feedback);
	shader.setUniform1f("opacityIn",inputOpacity);
	ofRect(0,0,frame.getWidth(),frame.getHeight());
	shader.end();
	back.getFboRef().end();

	front = VideoFrame::newVideoFrame(back);

	ofNotifyEvent(newFrameEvent,front);
}

float VideoFeedbackGPU::getFps(){
	return source1->getFps();
}
	
void VideoFeedbackGPU::setFeedback(float f)
{
	feedback = f;
}
	
	void VideoFeedbackGPU::setInputOpacity(float f)
	{
		inputOpacity = f;
	}
	
}
