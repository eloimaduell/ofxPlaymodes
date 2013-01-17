#pragma once

#include "ofMain.h"
#include "VideoBuffer.h"
#include "VideoHeader.h"
#include "VideoGrabber.h"
#include "VideoRenderer.h"
#include "VideoRate.h"
#include "BasicVideoRenderer.h"
#include "FileGrabber.h"
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
	
	
	
	////////////////////
	// ofxPlaymodes
	////////////////
	ofxPm::VideoGrabber			grabber;
	ofxPm::VideoBuffer			vBuffer;
	ofxPm::VideoHeader			vHeader;
	ofxPm::BasicVideoRenderer	vRendererBuffer,vRendererHeader;
	ofxPm::VideoRate			vRate;
	int							durationInFrames;
	ofVec2f						grabberResolution;
	float						grabberAspectRatio;

	// others
	bool						showGUI;
	bool						fullscreen;
	std::vector<ofVec2f>		arrayForDrawAudio;

	////////////////////
	// ofxGUI
	////////////////////
	
	ofxPanel		gui;
	
	ofxFloatSlider	delay;
	ofxFloatSlider	in;
	ofxFloatSlider	out;
	ofxFloatSlider	length;
	ofxFloatSlider	speed;
	ofxFloatSlider	feedBack;
	ofxFloatSlider	opacityOut;
	ofxFloatSlider	opacityIn;
	ofxFloatSlider	attack;
	ofxFloatSlider	decay;	
	ofxFloatSlider	dryWet;	
	ofxToggle		rec;
	ofxToggle		play;
	ofxToggle		opacityEnvToggle;
	ofxToggle		fullGrain;
	ofxToggle		drawAudioToggle;
	ofxToggle		grabberSettingsToggle;
	ofxIntSlider	scratchFactor;
	ofxIntSlider	drawAudioResolution;
	ofxIntSlider	avSyncOffset;
	ofxIntSlider	fps;
	ofxLabel		audioLabel;
	ofxLabel		presetLabel;
	ofxLabel		controlLabel;
	ofxLabel		setupLabel;

	// event changed float's
	////////////////////
	void inChanged(float &f);
	void outChanged(float &f);
	void lengthChanged(float &f);
	void speedChanged(float &f);
	void feedBackChanged(float &f);
	void opacityOutChanged(float &f);
	void opacityInChanged(float &f);
	void attackChanged(float &f);
	void decayChanged(float &f);
	void delayChanged(float &f);
	void dryWetChanged(float &f);
	
	// changed int's
	////////////////////
	void fpsChanged(int &i);
	void avSyncOffsetChanged(int &i);
	void scratchFactorChanged(int &i);
	// changed bool's
	////////////////////
	void recChanged(bool &b);
	void playChanged(bool &b);
	void drawAudioChanged(bool &b);
	void fullGrainChanged(bool &b);
	void opacityEnvToggleChanged(bool &b);
	void grabberSettingsToggleChanged(bool &b);
	
	float	oldRec;
	float	oldPlay;
	float	oldDelay;
	float	oldSpeed;
	float	oldScratch;
	float	oldOpacityOut;
	float	oldLength;
	bool	oldFullGrain;
	bool	oldOpacityEnvToggle;
	
	
	////////////////////
	// ofxPd
	////////////////////
	
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

	
	ofFbo		fboA;
	ofFbo		fboFeedback;
	ofImage		image;
	ofPixels	pixelsToBuffer;
	ofxPm::FileGrabber	videoGrabber;
	int		grabberInput;

	
	


};
