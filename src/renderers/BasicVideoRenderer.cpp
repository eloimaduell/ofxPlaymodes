/*
 * BasicVideoRenderer.cpp
 *
 *  Created on: 11/02/2012
 *      Author: arturo
 */

#include "BasicVideoRenderer.h"
#include "VideoFrame.h"


namespace ofxPm{
BasicVideoRenderer::BasicVideoRenderer(VideoSource & source) {
	this->source = &source;
	tex.allocate(this->source->width,this->source->height,GL_RGB);
	pbo.allocate(tex,1);
	updateTex=false;
	
}

BasicVideoRenderer::BasicVideoRenderer(){
	source = NULL;
}

void BasicVideoRenderer::setup(VideoSource & source)
{
	this->source = &source;
	
	tex.allocate(source.getWidth(),source.getHeight(),GL_RGB);
	pbo.allocate(tex,1);
	updateTex=false;
	
	printf("BasicVideoRenderer:: %d %d\n",source.getWidth(),source.getHeight());
}

BasicVideoRenderer::~BasicVideoRenderer() {
	// TODO Auto-generated destructor stub
}

void BasicVideoRenderer::draw(){
	draw(0,0,source->getWidth(),source->getHeight());
}
	
void BasicVideoRenderer::draw(int x,int y,int w,int h){
	VideoFrame frame = source->getNextVideoFrame();
	if(frame!=NULL)
	{
		pbo.loadData(frame.getPixelsRef());
		pbo.updateTexture();
		tex.draw(x,y,w,h);
	}
}
	

	
}
