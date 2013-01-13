/*
 * AudioFrame.h
 *
 *  Created on: 09-oct-2008
 *      Author: arturo castro
 */

#ifndef VIDEOHEADER_H_INCLUDED
#define VIDEOHEADER_H_INCLUDED

#include "VideoSink.h"
#include "VideoSource.h"
#include "VideoBuffer.h"

// acts as a video source and sink
// controls the position in a buffer
// based on speed, delay, fps and loop in/out

namespace ofxPm{
class VideoHeader:public VideoSink, public VideoSource{
public:
	
    VideoHeader(VideoBuffer & buffer);
    VideoHeader();
    ~VideoHeader();

    void setup(VideoBuffer & buffer);
    void draw();
    VideoFrame getNextVideoFrame();
	VideoFrame getVideoFrame(int index);
    int getNextPosition();
    VideoBuffer *getBuffer();
	
	// delay
    int		getDelayMs();
    int		getDelayFrames();
    double	getDelayPct() ;
    void	setDelayMs(double delay);
    void	setDelayFrames(int delay);
    void	setDelayPct(double pct);
	// in 
    double	getIn();
	double	getInFrames();
    void	setInMs(double in);
    void	setInFrames(int in);
    void	setInPct(double in);
	// out 
    double	getOut() ;	
	double	getOutFrames();
    void	setOutMs(double out);
    void	setOutFrames(int out);
    void	setOutPct(double out);
	// length
	double	getLength();
	double	getLengthFrames();
    void	setLengthMs(double length);
    void	setLengthFrames(int length);
    void	setLengthPct(double length);
	// speed
    double	getSpeed();
    void	setSpeed(double speed);
	// loop mode
	int		getLoopMode();
	void	setLoopMode(int loop);
	bool	isPlaying();
	void	setPlaying(bool loopMode);
	void	setPlaying(bool loopMode, double speed);
	void	setLoopToStart();
	
	// 
	int		getOpacity();
	void	setOpacity(int opacity);
    float	getFps();
    void	setFps(float fps);
	void	setDriveMode(int mode);
	
	// events
	void 	receivedLoopEvent(int &i);
	string	getInfo();
	
	// 
	void	setWindowPriority(string s);
	void	setOffsetInFrames(int o);

protected:
    VideoBuffer		*buffer;			// pointer to the buffer of video frames
	double			totalBufferSizeInMs;
	Timestamp		positionTS;
    TimeDiff		oneFrame;
    double			oneFrameMs;
	

    double			fps;				// framerate
    double			position;			// position expresses number of frames since start (0..N)
    int				currentPos;
    double 			speed;
    int 			delay;
    double 			in, out, length;	// expressed in ms from realtime ... in is bigger then out
	int				opacity;
	string			windowPriority;		// in : is always respected (changing length) length : length is always respected (constraining in)
	int				offsetFrames;				// to adjust av sync in frames
	
	//	OF_LOOP_NONE=0x01,
	//	OF_LOOP_PALINDROME=0x02,
	//	OF_LOOP_NORMAL=0x03

    bool			playing;
	int				loopMode;
	bool			loopStart;
	
	int				driveMode; // 0 : based on time // 1 : based on audio which controls it through delay param.
};
}
#endif // VIDEOHEADER_H_INCLUDED
