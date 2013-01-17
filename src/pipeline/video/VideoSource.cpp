/*
 * VideoSource.cpp
 *
 *  Created on: 09-oct-2008
 *      Author: arturo castro
 */

#include "VideoSource.h"

namespace ofxPm{
VideoSource::VideoSource() {
	//newFrameEvent.init("PlayModes.VideoSource.newFrameEvent");
	width=0;
	height=0;
}

VideoSource::~VideoSource() {
}

	void VideoSource::setWidth(int w)
	{
		width=w;
	}
	void VideoSource::setHeigth(int h)
	{
		height=h;
	}
	
	int VideoSource::getResolutionX()
	{
		return width;
	}
	int VideoSource::getResolutionY()
	{
		return height;
	}
	
}


