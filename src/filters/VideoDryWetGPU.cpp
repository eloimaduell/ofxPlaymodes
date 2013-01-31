/*
 * VideoFeedbackGPU.cpp
 *
 *  Created on: Jan 18, 2013
 *      Author: arturo
 */

#include "VideoDryWetGPU.h"
using Poco::ScopedLock;

#define STRINGIFY(...) #__VA_ARGS__

static string fragmentDryWetSrc =
#ifdef TARGET_LINUX
		"#version 140\n"
		"#extension GL_ARB_texture_rectangle : enable\n"
#endif
        STRINGIFY(
		uniform sampler2DRect tex0;
        uniform sampler2DRect tex1;
		uniform float dryWetA;		  
		uniform float dryWetB;		  
        void main (void){
            vec2 pos = gl_FragCoord.xy;

            //vec4 color = vec4(max(texture2DRect(tex0, pos).r* opacityIn,texture2DRect(tex1, pos).r*(feedback)),max(texture2DRect(tex0, pos).g*opacityIn,texture2DRect(tex1, pos).g*(feedback)),max(texture2DRect(tex0, pos).b*opacityIn,texture2DRect(tex1, pos).b*(feedback)),1.0);
			vec4 color = vec4(texture2DRect(tex0, pos)*dryWetA + texture2DRect(tex1, pos)*dryWetB);
				
            gl_FragColor = color;
        }
        );

namespace ofxPm{
VideoDryWetGPU::VideoDryWetGPU()
:source1(0)
,source2(0)
,newFrame(false)
{
	// TODO Auto-generated constructor stub

}

VideoDryWetGPU::~VideoDryWetGPU() {
	// TODO Auto-generated destructor stub
}

void VideoDryWetGPU::setup(VideoSource & _source1, VideoSource & _source2){
	source1 = &_source1;
	source2 = &_source2;
	front = VideoFrame::newVideoFrame(_source1.getNextVideoFrame());
	back = VideoFrame::newVideoFrame(_source1.getNextVideoFrame());
	back.setTextureOnly(true);
	ofAddListener(source1->newFrameEvent,this,&VideoDryWetGPU::newVideoFrame);
	shader.unload();
	shader.setupShaderFromSource(GL_FRAGMENT_SHADER,fragmentDryWetSrc);
	shader.linkProgram();

}
	
void VideoDryWetGPU::removeListener()
{
	ofRemoveListener(source1->newFrameEvent,this,&VideoDryWetGPU::newVideoFrame);
}
	

VideoFrame VideoDryWetGPU::getNextVideoFrame(){
	return front;
}

void VideoDryWetGPU::newVideoFrame(VideoFrame & frame){
	//front = VideoFrame::newVideoFrame(frame);

	if(source2->getNextVideoFrame()==NULL){
		ofNotifyEvent(newFrameEvent,front);
		return;
	}


	back.getFboRef().begin();
	shader.begin();
	shader.setUniformTexture("tex0",frame.getTextureRef(),0);
	shader.setUniformTexture("tex1",source2->getNextVideoFrame().getTextureRef(),1);
	shader.setUniform1f("dryWetA",dryWetA);
	shader.setUniform1f("dryWetB",dryWetB);
	ofRect(0,0,frame.getWidth(),frame.getHeight());
	shader.end();
	back.getFboRef().end();

	front = VideoFrame::newVideoFrame(back);

	ofNotifyEvent(newFrameEvent,front);
}

float VideoDryWetGPU::getFps(){
	return source1->getFps();
}
	
void VideoDryWetGPU::setDryWet(float dryWet)
{
	if(dryWet<0.5f)
	{
		dryWetB=dryWet*2.0;
		dryWetA=1.0;
	}
	else 
	{
		dryWetA=(dryWet-1.0f)*-2.0;
		dryWetB=1.0;
	}	
}
	
}
