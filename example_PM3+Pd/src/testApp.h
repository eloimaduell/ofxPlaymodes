#pragma once

#include "ofMain.h"
#include "VideoBuffer.h"
#include "VideoHeader.h"
#include "VideoGrabber.h"
#include "VideoRenderer.h"
#include "VideoRate.h"
#include "BasicVideoRenderer.h"
#include "ofxGui.h"
#include "ofxPd.h"

//pd externals ----------------------

#pragma once
extern "C" {
    void expr_setup();
}

#pragma once
extern "C" {
    void zexy_setup();
}

#pragma once
extern "C" {
    void susloop_tilde_setup();
}

//_pd externals ----------------------

class testApp : public pd::PdReceiver, public ofBaseApp{
public:
	void setup();
	void update();
	void draw();

	void keyPressed  (int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	
	void drawPdBuffer();
	
	
	bool		showGUI;
	bool		fullscreen;
	
	// ofxPlaymodes
	////////////////
	ofxPm::VideoGrabber			grabber;
	ofxPm::VideoBuffer			vBuffer;
	ofxPm::VideoHeader			vHeader;
	ofxPm::BasicVideoRenderer	vRenderer1,vRenderer2;
	ofxPm::VideoRate			vRate;
	ofVec2f						grabberResolution;
	float						grabberAspectRatio;

	// others
	std::vector<ofVec2f> arrayForDrawAudio;

	// ofxGUI
	//////////
	
	ofxPanel gui;
	ofxIntSlider fps;
	ofxIntSlider delay;
	ofxFloatSlider in;
	ofxFloatSlider out;
	ofxFloatSlider length;
	ofxFloatSlider speed;
	ofxFloatSlider feedBack;
	ofxFloatSlider opacityOut;
	ofxToggle rec;
	ofxToggle play;
	ofxToggle opacityEnvToggle;
	ofxToggle fullGrain;
	ofxIntSlider drawAudioResolution;
	ofxToggle drawAudioToggle;
	ofxIntSlider avSyncOffset;

	ofxIntSlider doppler;

	float	oldRec;
	float	oldPlay;
	float	oldDelay;
	float	oldSpeed;
	float	oldDoppler;
	float	oldOpacityOut;
	float	oldLength;
	bool	oldFullGrain;
	bool	oldOpacityEnvToggle;
	
	
	// ofxPd
	/////////
	int sampleRate;
	// the puredata instance
	ofxPd pd;
	string patchName;
	std::vector<float> arrayFromPd;
	// to be able to paint correctly the array
	int		indexArrayFromPd;

	// the audio callback
	void audioOut(float *output, int bufferSize, int numChannels);
	void audioIn(float *input, int bufferSize, int numChannels);

	void receiveFloat(const std::string& dest, float value);

	
	
	ofImage image;


};
