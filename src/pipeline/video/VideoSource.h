/*
 * VideoSource.h
 *
 *  Created on: 09-oct-2008
 *      Author: arturo castro
 */

#ifndef VIDEOSOURCE_H_
#define VIDEOSOURCE_H_

#include "VideoFrame.h"

namespace ofxPm{
class VideoSource{
public:
	VideoSource();
	virtual ~VideoSource();

	virtual VideoFrame getNextVideoFrame()=0;
    virtual float getFps()=0;
    ofEvent<VideoFrame> newFrameEvent;
	
	int width;
	int height;
	
	void setWidth(int w);
	void setHeigth(int h);
	
	int getWidth();
	int getHeight();
};
}
#endif /* VIDEOSOURCE_H_ */
