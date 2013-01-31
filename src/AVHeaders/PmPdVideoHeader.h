/*
 * AudioFrame.h
 *
 *  Created on: 09-oct-2008
 *      Author: arturo castro
 */

#ifndef PMPDVIDEOHEADER_H_INCLUDED
#define PMPDVIDEOHEADER_H_INCLUDED

#include "VideoHeader.h"

// PmPdVideoHeader
// added attack decay drawn

// classic VideHeader
// acts as a video source and sink
// controls the position in a buffer
// based on speed, delay, fps and loop in/out

namespace ofxPm{
class PmPdVideoHeader:public VideoHeader{
public:
	
    //PmPdVideoHeader(VideoBuffer & buffer);
    PmPdVideoHeader();
    ~PmPdVideoHeader();

    void draw();

	float getAttack();
	float getDecay();

	void setAttack(float f);
	void setDecay(float f);
	
protected:
	float	attack;
	float	decay;
	
};
	
}
#endif // VIDEOHEADER_H_INCLUDED
