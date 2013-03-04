#pragma once

#include "ofMain.h"
#include "ofxUI.h"
#include "pmThreadedOscSender.h"

class pmLFO {
	
	public:
	
	pmLFO();
	
	void setup(string _name,double _sampleRate, double _audioBufferSize,double bpm, int _xOrigin,string _oscIP);
	void update();
	
	
	string			name;
	double			sampleRate;
	double			audioBufferSize;
	double			phasor;
	double			bpm;
	double			phase;
	double			beatMult;
	double			beatDiv;
	double			waveFunc;
	double			n;
	double			sampleIncr;
	double			quantize;
	double			power;
	double			ampMin;
	double			ampMax;
	bool			onOff;
	int				selectedWaveform;
	vector<float> 	LFOBuffer;
	int				LFOBufferLength;
	bool			invertToggle;
	string			oscAddress;
	string			oscIP;
	bool			beatsFloats;

	int				xOrigin;
	
	// GUI
	void			setupGUI();
	void			setOscIP(string s);
	ofxUICanvas		*gui;
	void			setBPM(double bpm);
	void			guiEvent(ofxUIEventArgs &e);
	double			doubleMap(double value, double inputMin, double inputMax, double outputMin, double outputMax, bool clamp);
	void			resync();
	void			savePreset(int i);
	void			loadPreset(int i);
	
	
	// OSC 
	ofEvent<pmOscMessage>		newOscMessage;
	pmThreadedOscSender	pmOscSender;
	
};
