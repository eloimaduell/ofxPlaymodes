/*
 * VideoFeedbackGPU.cpp
 *
 *  Created on: Jan 18, 2013
 *      Author: arturo
 */

#include "VideoFeedbackPixelsGPU.h"
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
VideoFeedbackPixelsGPU::VideoFeedbackPixelsGPU()
:source1(0)
,source2(0)
,newFrame(false)
{
	// TODO Auto-generated constructor stub

}

VideoFeedbackPixelsGPU::~VideoFeedbackPixelsGPU() {
	// TODO Auto-generated destructor stub
}

void VideoFeedbackPixelsGPU::setup(VideoSource & _source1, VideoSource & _source2)
{
	source1 = &_source1;
	source2 = &_source2;
	VideoSource::width = source1->getWidth();
	VideoSource::height = source1->getHeight();

	
	// allocate and link tex's and pbo's
	tex1.allocate(source1->getWidth(),source1->getHeight(),GL_RGB);
	pbo1.allocate(tex1,1);
	tex2.allocate(source1->getWidth(),source1->getHeight(),GL_RGB);
	pbo2.allocate(tex2,1);
	// allocate fbo where to draw
	fbo.allocate(source1->getWidth(),source1->getHeight(),GL_RGB);
	
	printf("VideoFeedbackPixelsGPU:: source 1 %d %d \n",source1->getWidth(),source1->getHeight());
	
	front = VideoFrame::newVideoFrame(_source1.getNextVideoFrame());
	back = VideoFrame::newVideoFrame(_source1.getNextVideoFrame());
	//back.setTextureOnly(true);
	
	ofAddListener(source1->newFrameEvent,this,&VideoFeedbackPixelsGPU::newVideoFrame);
	
	shader.unload();
	shader.setupShaderFromSource(GL_FRAGMENT_SHADER,fragmentFeedbackSrc);
	shader.linkProgram();

}

void VideoFeedbackPixelsGPU::removeListener()
{
	ofRemoveListener(source1->newFrameEvent,this,&VideoFeedbackPixelsGPU::newVideoFrame);
	//stopThread();
}
	
	
VideoFrame VideoFeedbackPixelsGPU::getNextVideoFrame(){
	return front;
}

void VideoFeedbackPixelsGPU::newVideoFrame(VideoFrame & frame){

	if(source2->getNextVideoFrame()==NULL){
		ofNotifyEvent(newFrameEvent,front);
		return;
	}
	
	if(feedback==0.0)
	{
		front = VideoFrame::newVideoFrame(frame);
		ofNotifyEvent(newFrameEvent,front);
		return;
	}
	
	// pbo1 load data from source1 through frame param.
	pbo1.loadData(frame.getPixelsRef());
	pbo1.updateTexture();
	// pbo2 load data from source2
	pbo2.loadData(source2->getNextVideoFrame().getPixelsRef());
	pbo2.updateTexture();

	fbo.begin();
	shader.begin();
	shader.setUniformTexture("tex0",tex1,0);
	shader.setUniformTexture("tex1",tex2,1);
	shader.setUniform1f("feedback",feedback);
	shader.setUniform1f("opacityIn",inputOpacity);
	ofRect(0,0,frame.getWidth(),frame.getHeight());
	shader.end();
	fbo.end();

	// fast read of fbo result into pixels 
	ofPixels newPix;
	fboReader.readToPixels(fbo, newPix);
	front = VideoFrame::newVideoFrame(newPix);

	ofNotifyEvent(newFrameEvent,front);
}

float VideoFeedbackPixelsGPU::getFps(){
	return source1->getFps();
}
	
void VideoFeedbackPixelsGPU::setFeedback(float f)
{
	feedback = f;
}
	
void VideoFeedbackPixelsGPU::setInputOpacity(float f)
{
	inputOpacity = f;
}

}
