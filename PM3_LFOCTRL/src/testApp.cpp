#include "testApp.h"




//--------------------------------------------------------------
void testApp::setup()
{	
	bpm					= 120.0;
	sampleRate			= 96000.0;
	audioBufferSize		= 1024;
	oscIP				= "localhost";
	isShift				= false;
	hideGUIs			= false;
	lastTapTime			= -1.0;
	
	// GUI 
	setupGUI();

	// OSC
	pmOscSender.setup(oscIP,12345);
	ofAddListener(newOscMessage,&pmOscSender,&pmThreadedOscSender::pmSendMessage);

	
	// LFOs
	LFO_A.setup("LFO_A",sampleRate,audioBufferSize,bpm,215,"localhost");
	LFO_B.setup("LFO_B",sampleRate,audioBufferSize,bpm,490,"localhost");
	LFO_C.setup("LFO_C",sampleRate,audioBufferSize,bpm,765,"localhost");
	
	// Sound Stream
	soundStream.listDevices();
	soundStream.setDeviceID(0);
	soundStream.setup(this,0,2,sampleRate,audioBufferSize,1);
	
	//ofSetBackgroundAuto(false);
	//ofSetVerticalSync(true);
	ofBackground(50);
	ofSetFrameRate(25);
}

//--------------------------------------------------------------
void testApp::update()
{
	//ofSetFrameRate(25);
}

//--------------------------------------------------------------
void testApp::draw(){
	
}

//--------------------------------------------------------------
void testApp::audioOut(float *output, int bufferSize, int numChannels) 
{
}

//--------------------------------------------------------------
// AUDIO !!
//--------------------------------------------------------------

void testApp::audioIn(float *input, int bufferSize, int numChannels) 
{
	LFO_C.update();
	LFO_A.update();
	LFO_B.update();
}
//--------------------------------------------------------------
void testApp::keyPressed(int key){
	printf("keyPressed %d\n",key);
	
	ofxUIEventArgs ev;
	ofxUIButton *bR = (ofxUIButton *) guiBase->getWidget("Resync");
	ofxUIButton *b = (ofxUIButton *) guiBase->getWidget("Tap");

	
	switch (key) 
	{
		case 'f':
			ofToggleFullscreen();
			break;
		case OF_KEY_F1:
			hideGUIs=!hideGUIs;
			guiBase->setVisible(hideGUIs);
			guiParam->setVisible(hideGUIs);
			LFO_A.gui->setVisible(hideGUIs);
			LFO_B.gui->setVisible(hideGUIs);
			LFO_C.gui->setVisible(hideGUIs);
			break;
			
		case ' ':
			bR->setValue(true);
			ev = ofxUIEventArgs((ofxUIWidget *)bR);
			ofNotifyEvent(guiBase->newGUIEvent, ev, this);
			break;
		case OF_KEY_F2:			
			b->setValue(true);
			ev = ofxUIEventArgs((ofxUIWidget *)b);
			ofNotifyEvent(guiBase->newGUIEvent, ev, this);
			break;
	}
//		// 2 "
//		case 34:
//			LFO_A.savePreset(2);
//			LFO_B.savePreset(2);
//			LFO_C.savePreset(2);
//			break;
//		case '2':
//			LFO_A.loadPreset(2);
//			LFO_B.loadPreset(2);
//			LFO_C.loadPreset(2);
//			break;
//			
//		case OF_KEY_SHIFT:
//			isShift=true;
//			break;
//		default:
//			break;
//	}
	 
	
}
//--------------------------------------------------------------
void testApp::keyReleased(int key){
	ofxUIEventArgs ev;
	ofxUIButton *bR = (ofxUIButton *) guiBase->getWidget("Resync");
	ofxUIButton *b = (ofxUIButton *) guiBase->getWidget("Tap");
	switch (key) 
	{
		case ' ':
			bR->setValue(false);
			ev = ofxUIEventArgs((ofxUIWidget *)bR);
			ofNotifyEvent(guiBase->newGUIEvent, ev, this);
			break;
		case OF_KEY_F2:
			b->setValue(false);
			ev = ofxUIEventArgs((ofxUIWidget *)b);
			ofNotifyEvent(guiBase->newGUIEvent, ev, this);
			break;
			
	}
	
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}


//--------------------------------------------------------------
void testApp::setupGUI()
{
	float dim = 10; 
	float xInit = OFX_UI_GLOBAL_WIDGET_SPACING; 
	float length = 200-xInit; 

	guiBase = new ofxUICanvas(0, 0, length+xInit, ofGetHeight()*3); 
	guiBase->addWidgetDown(new ofxUILabel("PM3 LFO CTRL", OFX_UI_FONT_MEDIUM)); 
	guiBase->addSpacer(length-xInit, 2);
	guiBase->setDrawBack(false);
	//guiBase->addSlider("BPM", 0.0, 360.0, bpm, length-xInit,dim,OFX_UI_FONT_MEDIUM)->setIncrement(0.25);
	guiBase->addWidgetDown(new ofxUIButton( dim*2, dim*2, false, "Tap")); 
	guiBase->addWidgetRight(new ofxUINumberDialer(0.0,360.0,bpm,4,"BPM",OFX_UI_FONT_MEDIUM));
	guiBase->addSpacer(length-xInit, 2);
	guiBase->addWidgetDown(new ofxUIButton( dim*2, dim*2, false, "Resync")); 
	guiBase->addSpacer(length-xInit, 2);
	guiBase->addWidgetDown(new ofxUITextInput(length-xInit, "OSC IP", oscIP, OFX_UI_FONT_SMALL)); 
	guiBase->addSpacer(length-xInit, 2);
	guiBase->addWidgetDown(new ofxUIToggleMatrix(40, 40, 4, 4, "Presets")); 
   	ofxUIToggleMatrix* m = (ofxUIToggleMatrix*)guiBase->getWidget("Presets");
	m->setAllowMultiple(false);
	guiBase->addSpacer(length-xInit, 2);
	
	ofAddListener(guiBase->newGUIEvent,this,&testApp::guiEvent);

	
	// MAIN GUI PARAMETERS 
	
	guiParam = new ofxUIScrollableCanvas(0, 340, length+xInit, ofGetHeight()*3); 
	guiParam->addWidgetDown(new ofxUILabel("PM3 Params", OFX_UI_FONT_MEDIUM)); 
	guiParam->addSpacer(length-xInit, 2);
	guiParam->setDrawBack(false);
	guiParam->setDamping(0.0001);
	//guiParam->setScrollAreaToScreen();
	guiParam->setSnapping(false);



	
	guiParam->addSlider("Master In", 0.0, 1.0, 1.0, length-xInit,dim,OFX_UI_FONT_SMALL);
	guiParam->addSpacer(length-xInit, 2);
	guiParam->addSlider("Master Out", 0.0, 1.0, 1.0, length-xInit,dim,OFX_UI_FONT_SMALL);
	guiParam->addSpacer(length-xInit, 2);
	guiParam->addWidgetDown(new ofxUIToggle("Rec",false, dim, dim));
	guiParam->addWidgetRight(new ofxUIToggle("Play",false, dim, dim));
	
	
	// LOOPTYPE
	vector<string> loopTypeItems; 
    loopTypeItems.push_back("No Loop"); 
    loopTypeItems.push_back("Ping-Pong"); 
    loopTypeItems.push_back("Loop"); 
	//gui->addWidgetDown(new ofxUILabel("Wave function", OFX_UI_FONT_SMALL)); 
    guiParam->addWidgetRight(new ofxUIDropDownList(length/2, "Loop Type", loopTypeItems, OFX_UI_FONT_SMALL)); 
	ofxUIDropDownList *d = (ofxUIDropDownList*) guiParam->getWidget("Loop Type");
	d->setAutoClose(true);
	
	guiParam->addSpacer(length-xInit, 2);
	guiParam->addSlider("Speed", -16.0, 16.0, 1.0, length-xInit,dim,OFX_UI_FONT_SMALL)->setLabelPrecision(3);
	guiParam->addSpacer(length-xInit, 2);
	guiParam->addSlider("Delay", 0.0, 16000.0, 0.0, length-xInit,dim,OFX_UI_FONT_SMALL)->setLabelPrecision(3);
	guiParam->addSpacer(length-xInit, 2);
	guiParam->addSlider("In", 0.0, 16000.0, 16000.0, length-xInit,dim,OFX_UI_FONT_SMALL)->setLabelPrecision(3);
	guiParam->addSpacer(length-xInit, 2);
	guiParam->addSlider("Length", 0.0, 16000.0, 16000.0, length-xInit,dim,OFX_UI_FONT_SMALL)->setLabelPrecision(3);
	guiParam->addSlider("Feedback", 0.0, 1.0, 0.0, length-xInit,dim,OFX_UI_FONT_SMALL);
	guiParam->addSpacer(length-xInit, 2);
	guiParam->addSlider("Scratch Decay Ms", 0.0, 4000.0, 0.0, length-xInit,dim,OFX_UI_FONT_SMALL);
	guiParam->addSpacer(length-xInit, 2);	
	guiParam->addSlider("Pitch On Stretch", -48.0, 48.0, 0.0, length-xInit,dim,OFX_UI_FONT_SMALL);
	guiParam->addSpacer(length-xInit, 2);	
	guiParam->addWidgetDown(new ofxUIToggle("Time Stretch",false, dim, dim));
	guiParam->addSpacer(length-xInit, 2);	
	guiParam->addWidgetDown(new ofxUIToggle("Keep Whole Grain",false, dim, dim));
	guiParam->addSpacer(length-xInit, 2);	
	guiParam->addWidgetDown(new ofxUIFPS(OFX_UI_FONT_SMALL));
	guiParam->addSpacer(length-xInit, 2);	
	
	//guiParam->addSlider("Attack Grain", 0.0, 2000.0, 0.0, length-xInit,dim,OFX_UI_FONT_SMALL)->setLabelPrecision(3);
	//guiParam->addSpacer(length-xInit, 2);
	//guiParam->addSlider("Decay Grain", 0.0, 2000.0, 0.0, length-xInit,dim,OFX_UI_FONT_SMALL)->setLabelPrecision(3);
	//guiParam->addSpacer(length-xInit, 2);
	//guiParam->addSpacer(length-xInit, 2);
	//guiParam->addSlider("Pitch On Stretch",-16.0, 16.0, 1.0, length-xInit,dim,OFX_UI_FONT_SMALL);
	//guiParam->addSpacer(length-xInit, 2);
	//guiParam->addSlider("A/V Offset",-10.0, 10.0, 0.0, length-xInit,dim,OFX_UI_FONT_SMALL);
	//guiParam->addSpacer(length-xInit, 2);
	//guiParam->addSlider("Mic Sensitivity",0.0, 10.0, 1.0, length-xInit,dim,OFX_UI_FONT_SMALL);
	// time stretch + full grain + scratch
	
	ofAddListener(guiParam->newGUIEvent,this,&testApp::guiEvent);
	
}
//--------------------------------------------------------------
void testApp::guiEvent(ofxUIEventArgs &e)
{
	string name = e.widget->getName(); 
	int kind = e.widget->getKind(); 
	cout << "got event from: " << name << endl;

	
	if(name == "Resync")
	{
		ofxUIButton *t = (ofxUIButton*) e.widget;
		if(t->getValue()) 
		{
			LFO_A.resync();
			LFO_B.resync();
			LFO_C.resync();
		}
	}
	else if(name == "BPM")
	{
		ofxUINumberDialer *t = (ofxUINumberDialer*) e.widget;
		bpm=t->getValue();
		LFO_A.setBPM(bpm);
		LFO_B.setBPM(bpm);
		LFO_C.setBPM(bpm);
	}
	else if(name == "OSC IP")
	{
		ofxUITextInput *t = (ofxUITextInput*) e.widget;
		oscIP=t->getTextString();
		LFO_A.setOscIP(oscIP);
		LFO_B.setOscIP(oscIP);
		LFO_C.setOscIP(oscIP);
		pmOscSender.setup(oscIP,12345);

	}
	else if(name == "Presets(0,0)")
	{
		if(glutGetModifiers() & GLUT_ACTIVE_SHIFT) 
		{
			LFO_A.savePreset(1);
			LFO_B.savePreset(1);
			LFO_C.savePreset(1);
			guiParam->saveSettings("GUI/presets/Base1.xml");
		} else 
		{  
			LFO_A.loadPreset(1);
			LFO_B.loadPreset(1);
			LFO_C.loadPreset(1);
			this->loadPreset("GUI/presets/Base1.xml");
		} 
	}
	else if(name == "Presets(0,1)")
	{
		if(glutGetModifiers() & GLUT_ACTIVE_SHIFT) 
		{
			LFO_A.savePreset(2);
			LFO_B.savePreset(2);
			LFO_C.savePreset(2);
			guiParam->saveSettings("GUI/presets/Base2.xml");
		} else 
		{  
			LFO_A.loadPreset(2);
			LFO_B.loadPreset(2);
			LFO_C.loadPreset(2);
			this->loadPreset("GUI/presets/Base2.xml");
		}  
		
	}
	else if(name == "Presets(0,2)")
	{
		if(glutGetModifiers() & GLUT_ACTIVE_SHIFT) 
		{
			LFO_A.savePreset(3);
			LFO_B.savePreset(3);
			LFO_C.savePreset(3);
			guiParam->saveSettings("GUI/presets/Base3.xml");
		} else 
		{  
			LFO_A.loadPreset(3);
			LFO_B.loadPreset(3);
			LFO_C.loadPreset(3);
			this->loadPreset("GUI/presets/Base3.xml");

		}  

	}
	else if(name == "Presets(0,3)")
	{
		if(glutGetModifiers() & GLUT_ACTIVE_SHIFT) 
		{
			LFO_A.savePreset(4);
			LFO_B.savePreset(4);
			LFO_C.savePreset(4);
			guiParam->saveSettings("GUI/presets/Base4.xml");
		} else 
		{  
			LFO_A.loadPreset(4);
			LFO_B.loadPreset(4);
			LFO_C.loadPreset(4);
			this->loadPreset("GUI/presets/Base5.xml");

		}  

	}
	else if(name == "Presets(1,0)")
	{
		if(glutGetModifiers() & GLUT_ACTIVE_SHIFT) 
		{
			LFO_A.savePreset(5);
			LFO_B.savePreset(5);
			LFO_C.savePreset(5);
			guiParam->saveSettings("GUI/presets/Base5.xml");
		} else
		{  
			LFO_A.loadPreset(5);
			LFO_B.loadPreset(5);
			LFO_C.loadPreset(5);
			this->loadPreset("GUI/presets/Base5.xml");

		}  

	}
	else if(name == "Presets(1,1)")
	{
		if(glutGetModifiers() & GLUT_ACTIVE_SHIFT) 
		{
			LFO_A.savePreset(6);
			LFO_B.savePreset(6);
			LFO_C.savePreset(6);
			guiParam->saveSettings("GUI/presets/Base6.xml");
		} else 
		{  
			LFO_A.loadPreset(6);
			LFO_B.loadPreset(6);
			LFO_C.loadPreset(6);
			this->loadPreset("GUI/presets/Base6.xml");

		}  
	
	}
	else if(name == "Presets(1,2)")
	{
		if(glutGetModifiers() & GLUT_ACTIVE_SHIFT) 
		{
			LFO_A.savePreset(7);
			LFO_B.savePreset(7);
			LFO_C.savePreset(7);
			guiParam->saveSettings("GUI/presets/Base7.xml");
		} else 
		{  
			LFO_A.loadPreset(7);
			LFO_B.loadPreset(7);
			LFO_C.loadPreset(7);
			this->loadPreset("GUI/presets/Base7.xml");

		}  

	}
	else if(name == "Presets(1,3)")
	{
		if(glutGetModifiers() & GLUT_ACTIVE_SHIFT) 
		{
			LFO_A.savePreset(8);
			LFO_B.savePreset(8);
			LFO_C.savePreset(8);
			guiParam->saveSettings("GUI/presets/Base8.xml");
		} else 
		{  
			LFO_A.loadPreset(8);
			LFO_B.loadPreset(8);
			LFO_C.loadPreset(8);
			this->loadPreset("GUI/presets/Base8.xml");

		}  

	}
	else if(name == "Presets(2,0)")
	{
		if(glutGetModifiers() & GLUT_ACTIVE_SHIFT) 
		{
			LFO_A.savePreset(9);
			LFO_B.savePreset(9);
			LFO_C.savePreset(9);
			guiParam->saveSettings("GUI/presets/Base9.xml");
		} else 
		{  
			LFO_A.loadPreset(9);
			LFO_B.loadPreset(9);
			LFO_C.loadPreset(9);
			this->loadPreset("GUI/presets/Base9.xml");

		}  

	}
	else if(name == "Presets(2,1)")
	{
		if(glutGetModifiers() & GLUT_ACTIVE_SHIFT) 
		{
			LFO_A.savePreset(10);
			LFO_B.savePreset(10);
			LFO_C.savePreset(10);
			guiParam->saveSettings("GUI/presets/Base10.xml");
		} else 
		{  
			LFO_A.loadPreset(10);
			LFO_B.loadPreset(10);
			LFO_C.loadPreset(10);
			this->loadPreset("GUI/presets/Base10.xml");
		}  

	}
	else if(name == "Presets(2,2)")
	{
		if(glutGetModifiers() & GLUT_ACTIVE_SHIFT) 
		{
			LFO_A.savePreset(11);
			LFO_B.savePreset(11);
			LFO_C.savePreset(11);
			guiParam->saveSettings("GUI/presets/Base11.xml");
		} else 
		{  
			LFO_A.loadPreset(11);
			LFO_B.loadPreset(11);
			LFO_C.loadPreset(11);
			this->loadPreset("GUI/presets/Base11.xml");

		}  

	}
	else if(name == "Presets(2,3)")
	{
		if(glutGetModifiers() & GLUT_ACTIVE_SHIFT) 
		{
			LFO_A.savePreset(12);
			LFO_B.savePreset(12);
			LFO_C.savePreset(12);
			guiParam->saveSettings("GUI/presets/Base12.xml");
		} else 
		{  
			LFO_A.loadPreset(12);
			LFO_B.loadPreset(12);
			LFO_C.loadPreset(12);
			this->loadPreset("GUI/presets/Base12.xml");

		}  

	}
	else if(name == "Presets(3,0)")
	{
		if(glutGetModifiers() & GLUT_ACTIVE_SHIFT) 
		{
			LFO_A.savePreset(13);
			LFO_B.savePreset(13);
			LFO_C.savePreset(13);
			guiParam->saveSettings("GUI/presets/Base13.xml");
		} else 
		{  
			LFO_A.loadPreset(13);
			LFO_B.loadPreset(13);
			LFO_C.loadPreset(13);
			this->loadPreset("GUI/presets/Base13.xml");

		}  

	}
	else if(name == "Presets(3,1)")
	{
		if(glutGetModifiers() & GLUT_ACTIVE_SHIFT) 
		{
			LFO_A.savePreset(14);
			LFO_B.savePreset(14);
			LFO_C.savePreset(14);
			guiParam->saveSettings("GUI/presets/Base14.xml");

		} else 
		{  
			LFO_A.loadPreset(14);
			LFO_B.loadPreset(14);
			LFO_C.loadPreset(14);
			this->loadPreset("GUI/presets/Base14.xml");

		}  
		
	}
	else if(name == "Presets(3,2)")
	{
		if(glutGetModifiers() & GLUT_ACTIVE_SHIFT) 
		{
			LFO_A.savePreset(15);
			LFO_B.savePreset(15);
			LFO_C.savePreset(15);
			guiParam->saveSettings("GUI/presets/Base15.xml");

		} else 
		{  
			LFO_A.loadPreset(15);
			LFO_B.loadPreset(15);
			LFO_C.loadPreset(15);
			this->loadPreset("GUI/presets/Base15.xml");

		}  
	}
	else if(name == "Presets(3,3)")
	{
		if(glutGetModifiers() & GLUT_ACTIVE_SHIFT) 
		{
			LFO_A.savePreset(16);
			LFO_B.savePreset(16);
			LFO_C.savePreset(16);
			guiParam->saveSettings("GUI/presets/Base16.xml");

		} else 
		{  
			LFO_A.loadPreset(16);
			LFO_B.loadPreset(16);
			LFO_C.loadPreset(16);
			this->loadPreset("GUI/presets/Base16.xml");

		}  
		
	}
	////////////
	// BOOLS
	////////////
	else if(name == "Play")
	{
		ofxUIToggle* t = (ofxUIToggle*) e.widget;
		// send OSC 
		pmOscMessage pmM;
		if(t->getValue()) pmM.value = 1.0;
		else pmM.value = 0.0;
		pmM.address="/pm3/header/play";
		ofNotifyEvent(newOscMessage,pmM,this);		
	}
	else if(name == "Rec")
	{
		ofxUIToggle* t = (ofxUIToggle*) e.widget;
		// send OSC 
		pmOscMessage pmM;
		if(t->getValue()) pmM.value = 1.0;
		else pmM.value = 0.0;
		pmM.address="/pm3/header/rec";
		ofNotifyEvent(newOscMessage,pmM,this);		
	}
	else if(name == "Time Stretch")
	{
		ofxUIToggle* t = (ofxUIToggle*) e.widget;
		// send OSC 
		pmOscMessage pmM;
		if(t->getValue()) pmM.value = 1.0;
		else pmM.value = 0.0;
		pmM.address="/pm3/header/enableTimeStretch";
		ofNotifyEvent(newOscMessage,pmM,this);		
	}
	else if(name == "Keep Whole Grain")
	{
		ofxUIToggle* t = (ofxUIToggle*) e.widget;
		// send OSC 
		pmOscMessage pmM;
		if(t->getValue()) pmM.value = 1.0;
		else pmM.value = 0.0;
		pmM.address="/pm3/header/keepWholeGrain";
		ofNotifyEvent(newOscMessage,pmM,this);		
	}
	////////////
	// SLIDERS
	////////////
	else if(name == "Master In")
	{
		ofxUISlider* t = (ofxUISlider*) e.widget;
		// send OSC 
		pmOscMessage pmM;
		pmM.value = t->getScaledValue();
		pmM.address="/pm3/master/in";
		ofNotifyEvent(newOscMessage,pmM,this);		
	}
	else if(name == "Master Out")
	{
		ofxUISlider* t = (ofxUISlider*) e.widget;
		// send OSC 
		pmOscMessage pmM;
		pmM.value = t->getScaledValue();
		pmM.address="/pm3/master/out";
		ofNotifyEvent(newOscMessage,pmM,this);		
	}
	else if(name == "Speed")
	{
		ofxUISlider* t = (ofxUISlider*) e.widget;
		// send OSC 
		pmOscMessage pmM;
		pmM.value = t->getScaledValue();
		pmM.address="/pm3/header/speed";
		ofNotifyEvent(newOscMessage,pmM,this);		
	}
	else if(name == "Delay")
	{
		ofxUISlider* t = (ofxUISlider*) e.widget;
		// send OSC 
		pmOscMessage pmM;
		pmM.value = t->getScaledValue();
		pmM.address="/pm3/header/delay";
		ofNotifyEvent(newOscMessage,pmM,this);		
	}
	else if(name == "In")
	{
		ofxUISlider* t = (ofxUISlider*) e.widget;
		// send OSC 
		pmOscMessage pmM;
		pmM.value = t->getScaledValue();
		pmM.address="/pm3/header/in";
		ofNotifyEvent(newOscMessage,pmM,this);		
	}
	else if(name == "Length")
	{
		ofxUISlider* t = (ofxUISlider*) e.widget;
		// send OSC 
		pmOscMessage pmM;
		pmM.value = t->getScaledValue();
		pmM.address="/pm3/header/length";
		ofNotifyEvent(newOscMessage,pmM,this);		
	}
	else if(name == "Feedback")
	{
		ofxUISlider* t = (ofxUISlider*) e.widget;
		// send OSC 
		pmOscMessage pmM;
		pmM.value = t->getScaledValue();
		pmM.address="/pm3/buffer/feedback";
		ofNotifyEvent(newOscMessage,pmM,this);		
	}
	else if(name == "Scratch Decay Ms")
	{
		ofxUISlider* t = (ofxUISlider*) e.widget;
		// send OSC 
		pmOscMessage pmM;
		pmM.value = t->getScaledValue();
		pmM.address="/pm3/buffer/scratchDecayTime";
		ofNotifyEvent(newOscMessage,pmM,this);		
	}
	
	else if(name== "Tap")
	{
		
		ofxUIButton *t = (ofxUIButton*) e.widget;
		if(t->getValue()) 
		{
			float nowIs = ofGetElapsedTimeMillis();
			if(nowIs-lastTapTime>2500.0)
			{
				tapBpmVector.clear();
				//printf("clear BPM!! now is %f last was %f :: diff %f \n",nowIs,lastTapTime,nowIs-lastTapTime);
				lastTapTime = nowIs;
			}
			else
			{
				float timeNow = ofGetElapsedTimeMillis();
				float lengthOfBeat =  timeNow - lastTapTime ;
				lastTapTime = timeNow;
				//printf(">> lengthOfBeat %f\n",lengthOfBeat);
	//			if(lengthOfBeat)
				tapBpmVector.push_back(60.0f / (lengthOfBeat/1000.0));			
			}
			float mediaBpm =0;
			for(int i=0;i<tapBpmVector.size();i++)
			{
				mediaBpm = mediaBpm + tapBpmVector[i];
			}
			if (tapBpmVector.size()>0) 
			{
				//printf("BPM_VECTOR = %f\n",mediaBpm/tapBpmVector.size());
				ofxUINumberDialer* n = (ofxUINumberDialer*) guiBase->getWidget("BPM");
				ofxUIEventArgs ev;
				n->setValue(int(mediaBpm/float(tapBpmVector.size())));
				ev = ofxUIEventArgs((ofxUIWidget *)n);
				ofNotifyEvent(guiBase->newGUIEvent, ev, this);
			}
	
		}
	}
	
	
	else if(name=="Loop Type")
	{
		ofxUIDropDownList* wf = (ofxUIDropDownList *) e.widget;
		
		vector<ofxUIWidget *> &selected = wf->getSelected(); 
		string selectedName;
		for(int i = 0; i < selected.size(); i++)
		{
			selectedName=selected[i]->getName();
			wf->setLabelText(selectedName);			
		}
		
		// send OSC 
		pmOscMessage pmM;
		
		if(selectedName=="No Loop")
		{
			pmM.value = 1;
		}
		else if (selectedName=="Ping-Pong")
		{
			pmM.value = 2;
		}
		else if (selectedName=="Loop")
		{
			pmM.value = 3;
		}
		pmM.address="/pm3/header/loopType";
		ofNotifyEvent(newOscMessage,pmM,this);		
	
	}
}

//--------------------------------------------------------------

void testApp::loadPreset(string s)
{

	guiParam->loadSettings(s);
	
	ofxUIDropDownList* wf = (ofxUIDropDownList *) guiParam->getWidget("Loop Type"); 
	vector<ofxUIWidget *> &selected = wf->getSelected(); 
	string selectedName;
	
	printf("loading preset selected loop type %d\n",selected.size());
	
	for(int i = 0; i < selected.size(); i++)
	{
		selectedName=selected[i]->getName();

		printf("!!%d __ %s",i,selectedName.c_str());
		
		wf->setLabelText(selectedName);
		
//		if(selectedName=="No Loop")
//		{
//			selectedWaveform=0;
//		}
//		else if(selectedName=="Cos")
//		{
//			selectedWaveform=1;
//		}
//		else if(selectedName=="Square")
//		{
//			selectedWaveform=2;
//		}
//		else if(selectedName=="Saw")
//		{
//			selectedWaveform=3;
//		}
//		else if(selectedName=="Triangle")
//		{
//			selectedWaveform=4;
//		}
//		else if(selectedName=="Random")
//		{
//			selectedWaveform=5;
//		}
		
	}


}
