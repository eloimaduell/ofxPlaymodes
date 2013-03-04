/*
 * VideoFeedbackGPU.cpp
 *
 *  Created on: Jan 18, 2013
 *      Author: arturo
 */

#include "VideoDryWetPixelsGPU.h"
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
VideoDryWetPixelsGPU::VideoDryWetPixelsGPU()
:source1(0)
,source2(0)
,newFrame(false)
{
	// TODO Auto-generated constructor stub

}

VideoDryWetPixelsGPU::~VideoDryWetPixelsGPU() {
	// TODO Auto-generated destructor stub
}

void VideoDryWetPixelsGPU::setup(VideoSource & _source1, VideoSource & _source2){
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
	
	printf("VideoDryWetPixelsGPU:: source 1 %d %d \n",source1->getWidth(),source1->getHeight());	
	
	front = VideoFrame::newVideoFrame(_source1.getNextVideoFrame());
	back = VideoFrame::newVideoFrame(_source1.getNextVideoFrame());
	
	//back.setTextureOnly(true);
	
	ofAddListener(source1->newFrameEvent,this,&VideoDryWetPixelsGPU::newVideoFrame);
	
	shader.unload();
	shader.setupShaderFromSource(GL_FRAGMENT_SHADER,fragmentDryWetSrc);
	shader.linkProgram();

}
	
void VideoDryWetPixelsGPU::removeListener()
{
	ofRemoveListener(source1->newFrameEvent,this,&VideoDryWetPixelsGPU::newVideoFrame);
}
	

VideoFrame VideoDryWetPixelsGPU::getNextVideoFrame(){
	return front;
}

void VideoDryWetPixelsGPU::newVideoFrame(VideoFrame & frame){
	//front = VideoFrame::newVideoFrame(frame);

	if(source2->getNextVideoFrame()==NULL){
		ofNotifyEvent(newFrameEvent,front);
		return;
	}
	// try to avoid the use of pbo in case we're in this 2 special cases where
	// no need to use the pbo's, shader, fbo ... 
	if((dryWetA==1.0) && (dryWetB==0.0))
	{
		front = VideoFrame::newVideoFrame(frame); 
		ofNotifyEvent(newFrameEvent,front);
		
		return;
	}
	else if ((dryWetA==0.0) && (dryWetB==1.0))
	{
		front = VideoFrame::newVideoFrame(source2->getNextVideoFrame()); 
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
//	shader.setUniformTexture("tex0",frame.getTextureRef(),0);
//	shader.setUniformTexture("tex1",source2->getNextVideoFrame().getTextureRef(),1);
	shader.setUniformTexture("tex0",tex1,0);
	shader.setUniformTexture("tex1",tex2,1);
	shader.setUniform1f("dryWetA",dryWetA);
	shader.setUniform1f("dryWetB",dryWetB);
	ofRect(0,0,frame.getWidth(),frame.getHeight());
	shader.end();
	fbo.end();
	
	// now we create the new frame from the fbo itself 
	// dryWet just is for drawing on screen, no need to get back to pixels
	front = VideoFrame::newVideoFrame(fbo);	
	ofNotifyEvent(newFrameEvent,front);
}

float VideoDryWetPixelsGPU::getFps(){
	return source1->getFps();
}
	
void VideoDryWetPixelsGPU::setDryWet(float dryWet)
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
