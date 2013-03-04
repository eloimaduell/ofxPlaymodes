#pragma once

#include "ofMain.h"
#include "ofxUI.h"
#include "pmLFO.h"
#include "pmThreadedOscSender.h"


#ifdef TARGET_WIN32
#define GLUT_BUILDING_LIB
#include "glut.h"
#endif

#ifdef TARGET_OSX
#include "../../../libs/glut/lib/osx/GLUT.framework/Versions/A/Headers/glut.h"
#include <Carbon/Carbon.h>
#endif

#ifdef TARGET_LINUX
#include <GL/glut.h>
#endif

class testApp : public ofBaseApp
{

public:

	void setup();
	void update();
	void draw();
	
	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	
	// the audio callback
	void audioOut(float *output, int bufferSize, int numChannels);
	void audioIn(float *input, int bufferSize, int numChannels);
	
	ofSoundStream			soundStream;
	double					sampleRate;
	double					audioBufferSize;

	// GUI
	double					bpm;
	bool					sync;
	string					oscIP;
	
	void					setupGUI();
	ofxUICanvas				*guiBase;
	ofxUIScrollableCanvas	*guiParam;
	void					guiEvent(ofxUIEventArgs &e);
	double					doubleMap(double value, double inputMin, double inputMax, double outputMin, double outputMax, bool clamp);

	void					loadPreset(string s);
	
	pmLFO					LFO_A;
	pmLFO					LFO_B;
	pmLFO					LFO_C;
	
	bool					isShift;
	bool					hideGUIs;
	// OSC 
	ofEvent<pmOscMessage>	newOscMessage;
	pmThreadedOscSender		pmOscSender;
	
	
	// TAP TEMPO
	vector<float>			tapBpmVector;
	double					lastTapTime;
	
	
};
