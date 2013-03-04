#include "pmLFO.h"

//--------------------------------------------------------------
pmLFO::pmLFO()
{
	name="defaultName";
	sampleRate=96000;
	audioBufferSize=1024;
	phasor=0.0;
	bpm=0.0;
	phase=0.0;
	beatMult=0;
	beatDiv=0;
	waveFunc=0;
	n=0.0;
	sampleIncr=1.0;
	quantize=0.0;
	power=0.0;
	ampMin=0.0;
	ampMax=0.0;
	onOff=false;
	selectedWaveform=0;
	//LFOBuffer;
	LFOBufferLength=100;
	invertToggle=false;
	oscAddress="/pm3/";
	oscIP="localhost";
	beatsFloats=true;
	
}


//--------------------------------------------------------------
void pmLFO::setup(string _name,double _sampleRate, double _audioBufferSize, double _bpm, int _xOrigin,string _oscIP)
{	
	name				= _name;
	bpm					= _bpm;
	phasor				= 0.0;
	sampleRate			= _sampleRate;
	audioBufferSize		= _audioBufferSize;
	sampleIncr			= 1.0;
	beatMult			= 1.0;
	beatDiv				= 1.0;
	quantize			= 0.0;
	power				= 0.0;
	waveFunc			= 0.0;
	oscAddress			= "/pm3/";
	xOrigin				= _xOrigin;
	onOff				= false;
	oscIP				= _oscIP;
	
	// GUI 
	setupGUI();
	
	pmOscSender.setup(oscIP,12345);
	ofAddListener(newOscMessage,&pmOscSender,&pmThreadedOscSender::pmSendMessage);
	
	
/*	
	//----------------------------------------------------
	// register any method of any class to an event.
	// the method must provide one of the following
	// signatures:
	//     void method(ArgumentsType & args)
	//     void method(const void * sender, ArgumentsType &args)
	// ie:
	//     ofAddListener(addon.newIntEvent, this, &Class::method)
	template <class EventType,typename ArgumentsType, class ListenerClass>
	static void ofAddListener(EventType & event, ListenerClass  * listener, void (ListenerClass::*listenerMethod)(const void*, ArgumentsType&)){
		event -= Poco::delegate(listener, listenerMethod);
		event += Poco::delegate(listener, listenerMethod);
	}
	
	template <class EventType,typename ArgumentsType, class ListenerClass>
	static void ofAddListener(EventType & event, ListenerClass  * listener, void (ListenerClass::*listenerMethod)(ArgumentsType&)){
		event -= Poco::delegate(listener, listenerMethod);
		event += Poco::delegate(listener, listenerMethod);
	}
	
*/	
	
}

//--------------------------------------------------------------
void pmLFO::update() 
{
	if(onOff)
	{
		double newRandom;
		for(int i=0;i<audioBufferSize;i++)
		{
			n = n + sampleIncr*(beatMult/beatDiv);
			phasor=doubleMap(n*(1.0/sampleRate), 0.0, (60.0/bpm), 0.0, 1.0, false);
			
			ofxUIEventArgs ev;
			ofxUIButton *b = (ofxUIButton *) gui->getWidget("B");
			
			if(phasor>1.0)
			{
				phasor=0.0;
				n=0;
				// random retrigg
				if(selectedWaveform==5) newRandom =ofRandom(2.0)-1.0;
				// bpm monitor 
				b->setValue(true);
				ev = ofxUIEventArgs((ofxUIWidget *)b);
				ofNotifyEvent(gui->newGUIEvent, ev, this);
			}
			
			else if((phasor>0.1) && (b->getValue()))
			{
				b->setValue(false);
				ev = ofxUIEventArgs((ofxUIWidget *)b);
				ofNotifyEvent(gui->newGUIEvent, ev, this);				
			}
		}
		
		if (selectedWaveform==0) 
		{
			// SIN
			waveFunc = sin((phasor+phase)*2.0*PI);
		}
		else if (selectedWaveform==1)
		{
			// COS
			waveFunc = cos((phasor+phase)*2.0*PI);
		}
		else if (selectedWaveform==2) 
		{
			// SQUARE
			if(fmod(phasor+phase,1.0)>doubleMap(power,-1.0,1.0,0.0,1.0,false)) waveFunc=-1.0;
			else waveFunc=1.0;
		}
		else if (selectedWaveform==3) 
		{
			// SAW
			waveFunc=(fmod(phasor+phase,1.0)*2.0)-1.0;
		}
		else if (selectedWaveform==4) 
		{
			// TRIANGLE
			if(fmod(phasor+phase,1.0)>0.5) waveFunc=1.0-((fmod(phasor+phase,1.0)-0.5)*4);
			else waveFunc=-1.0+fmod(phasor+phase,1.0)*4;		
		}
		else if (selectedWaveform==5) 
		{
			// RANDOM
			waveFunc = newRandom;
		}

		if(invertToggle) waveFunc=-waveFunc;

		// re mapp values ti 0.0 1.0 !! 
		waveFunc = doubleMap(waveFunc,-1.0,1.0,0.0,1.0,false);
		
		// power
		if (power>=0.0)
		{
			waveFunc = pow(waveFunc,doubleMap(power,0.0,1.0,1.0,16.0,false));
		}
		else 
		{
			waveFunc = pow(waveFunc,doubleMap(power,-1.0,0.0,0.0,1.0,false));		
		}

		
		// quantize the values
		if(quantize>=1.0)
		{
			waveFunc = double(floor(waveFunc*(ceil(quantize-1)))) / (floor(quantize));
		}
		
		ofxUIMovingGraph* mg = (ofxUIMovingGraph *) gui->getWidget("WaveDisplay"); 
		mg->addPoint(waveFunc);

		
		// send OSC 
		pmOscMessage pmM;
		if(beatsFloats) pmM.value=ofMap(waveFunc,0.0,1.0,float(60000.0/bpm)*ampMin,float(60000.0/bpm)*ampMax, true);
		else pmM.value=ofMap(waveFunc,0.0,1.0,ampMin,ampMax, true);
		ofxUITextInput* t = (ofxUITextInput*) gui->getWidget("OSC address");
		pmM.address=t->getTextString();
		ofNotifyEvent(newOscMessage,pmM,this);
		
	}
}
//--------------------------------------------------------------
void pmLFO::resync()
{
	n=0;
	phasor=0;
}


/*
//--------------------------------------------------------------
void testApp::keyPressed(int key){
	printf("keyPressed %d\n",key);
	switch (key) {
		// 1 !
		case 33:
			gui->saveSettings("GUI/guiSettings1.xml");
			break;
		case '1':
			loadPreset("GUI/guiSettings1.xml");
			resync();
			break;
		// 2 "
		case 34:
			gui->saveSettings("GUI/guiSettings2.xml"); 
			break;
		case '2':
			loadPreset("GUI/guiSettings2.xml"); 			
			resync();
			break;
		// 3 á
		case 194:
			gui->saveSettings("GUI/guiSettings3.xml");
			break;
		case '3':
			loadPreset("GUI/guiSettings3.xml"); 
			resync();
			break;
		// 4 $
		case 36:
			gui->saveSettings("GUI/guiSettings4.xml"); 
			break;
		case '4':
			loadPreset("GUI/guiSettings4.xml"); 			
			resync();
			break;
		// 5 %
		case '%':
			gui->saveSettings("GUI/guiSettings5.xml");
			break;
		case '5':
			loadPreset("GUI/guiSettings5.xml"); 
			resync();
			break;
		// 6 &
		case '&':
			gui->saveSettings("GUI/guiSettings6.xml"); 
			break;
		case '6':
			loadPreset("GUI/guiSettings6.xml"); 			
			resync();
			break;
		// 7 /
		case 47:
			gui->saveSettings("GUI/guiSettings7.xml");
			printf("!!save 7\n");
			break;
		case '7':
			loadPreset("GUI/guiSettings7.xml"); 
			resync();
			break;
		// 8 ( 
		case '(':
			gui->saveSettings("GUI/guiSettings8.xml"); 
			printf("!!save 8\n");
			break;
		case '8':
			loadPreset("GUI/guiSettings8.xml"); 			
			resync();
			break;
		default:
			break;
	}
	
}
*/

//--------------------------------------------------
double pmLFO::doubleMap(double value, double inputMin, double inputMax, double outputMin, double outputMax, bool clamp) 
{
	
	if (fabs(inputMin - inputMax) < FLT_EPSILON){
		ofLog(OF_LOG_WARNING, "ofMap: avoiding possible divide by zero, check inputMin and inputMax\n");
		return outputMin;
	} else {
		double outVal = ((value - inputMin) / (inputMax - inputMin) * (outputMax - outputMin) + outputMin);
		
		if( clamp ){
			if(outputMax < outputMin){
				if( outVal < outputMax )outVal = outputMax;
				else if( outVal > outputMin )outVal = outputMin;
			}else{
				if( outVal > outputMax )outVal = outputMax;
				else if( outVal < outputMin )outVal = outputMin;
			}
		}
		return outVal;
	}
	
}

//--------------------------------------------------------------
void pmLFO::setupGUI()
{
	float dim = 16; 
	float xInit = OFX_UI_GLOBAL_WIDGET_SPACING; 
	float length = 260-xInit; 
	selectedWaveform=-1;
	invertToggle=false;

	gui = new ofxUICanvas(xOrigin, 0, length+xInit, ofGetHeight()); 
	gui->addWidgetDown(new ofxUILabel(name, OFX_UI_FONT_MEDIUM)); 
	gui->addWidgetRight(new ofxUIButton( dim, dim, false,"B",OFX_UI_FONT_SMALL),OFX_UI_ALIGN_RIGHT);
	ofxUIButton* b= (ofxUIButton*) gui->getWidget("B");
	b->setLabelVisible(false);
	gui->addSpacer(length-xInit, 2);
	gui->setDrawBack(false);
	// BPM MONITOR
	gui->addWidgetDown(new ofxUIToggle( dim, dim, false, "On / Off")); 
	gui->addSpacer(length-xInit, 2);
	// INVERT 
	gui->addWidgetDown(new ofxUIToggle( dim, dim, false, "Invert")); 
	gui->addSpacer(length-xInit, 2);
	// BPM
	//gui->addSlider("BPM", 0.0, 360.0, bpm, length-xInit,dim)->setLabelPrecision(4);;
	//gui->addSpacer(length-xInit, 2);
	// BEAT MULT
	gui->addSlider("Beat Mult", 1,16, beatMult, length-xInit,dim)->setLabelPrecision(0);	
	gui->addSpacer(length-xInit, 2);
	// BEAT DIV
	gui->addSlider("Beat Div", 1,64, beatDiv, length-xInit,dim)->setLabelPrecision(0);
	gui->addSpacer(length-xInit, 2);
	// PHASE
	gui->addSlider("Phase", 0.0, 1.0, phase, length-xInit,dim);
	gui->addSpacer(length-xInit, 2);
	// PHASE
	gui->addSlider("Quantize", 0.0, 64.0, quantize, length-xInit,dim);//->setLabelPrecision();
	gui->addSpacer(length-xInit, 2);
	// POWER
	gui->addSlider("Power", -1.0, 1.0, power, length-xInit,dim);//->setLabelPrecision();
	gui->addSpacer(length-xInit, 2);

	// WAVEFORM
	vector<string> waveformItems; 
    waveformItems.push_back("Sin"); 
    waveformItems.push_back("Cos"); 
    waveformItems.push_back("Square"); 
    waveformItems.push_back("Saw"); 
    waveformItems.push_back("Triangle"); 
    waveformItems.push_back("Random");      
    
	gui->addWidgetDown(new ofxUILabel("Wave function", OFX_UI_FONT_SMALL)); 
    gui->addWidgetDown(new ofxUIDropDownList(length, "Waveform", waveformItems, OFX_UI_FONT_SMALL)); 
	ofxUIDropDownList* wf = (ofxUIDropDownList *) gui->getWidget("Waveform"); 
	wf->setAutoClose(true);

	// WAVEFORM DISPLAY
	LFOBufferLength = 255-xInit;
	for(int i = 0; i < LFOBufferLength; i++)
    {
        LFOBuffer.push_back(0.0);
    }
	gui->addSpacer(length-xInit, 2);
	gui->addWidgetDown(new ofxUIMovingGraph(length-xInit, 128, LFOBuffer, LFOBufferLength, 0, 1.0, "WaveDisplay")); 
	gui->addSpacer(length-xInit, 2);
	
	// AMP MIN & MAX
	//ofxUINumberDialer(float _min, float _max, float *_value, int _precision, string _name, int _size)
	gui->addWidgetDown(new ofxUILabel("<-", OFX_UI_FONT_SMALL)); 
	gui->addWidgetRight(new ofxUINumberDialer(-16000.0,16000.0,ampMin,4,"min",OFX_UI_FONT_SMALL));
	gui->addWidgetRight(new ofxUILabel("+>", OFX_UI_FONT_SMALL)); 
	gui->addWidgetRight(new ofxUINumberDialer(-16000.0,16000.0,ampMax,4,"max",OFX_UI_FONT_SMALL));
	
	gui->addSpacer(length-xInit, 2);
	gui->addWidgetDown(new ofxUIToggle( dim, dim, beatsFloats, "Beats / Floats"));
	gui->addSpacer(length-xInit, 2);
	//ofxUITextInput(string _name, string _textstring, float w, float h = 0, float x = 0, float y = 0, int _size = OFX_UI_FONT_MEDIUM) : ofxUIWidgetWithLabel()
	gui->addWidgetDown(new ofxUITextInput(length-xInit, "OSC address", oscAddress, OFX_UI_FONT_SMALL)); 
		
	ofAddListener(gui->newGUIEvent,this,&pmLFO::guiEvent);
	
}
//--------------------------------------------------------------
void pmLFO::guiEvent(ofxUIEventArgs &e)
{
	string name = e.widget->getName(); 
	int kind = e.widget->getKind(); 
	//cout << "got event from: " << name << endl;
	
	if(name == "On / Off")
	{
		ofxUIToggle *t = (ofxUIToggle*) e.widget;
		onOff=t->getValue();
	}
//	else if(name == "BPM")
//	{
//		ofxUISlider *slider = (ofxUISlider *) e.widget; 
//		cout << "bpm " << slider->getScaledValue() << endl; 
//		bpm = slider->getScaledValue(); 
//	}
	else if(name == "Phase")
	{
		ofxUISlider *slider = (ofxUISlider *) e.widget; 
		phase = slider->getScaledValue(); 
	}
	else if(name == "Beat Mult")
	{
		ofxUISlider *slider = (ofxUISlider *) e.widget; 
		beatMult = int(slider->getScaledValue()); 
	}
	else if(name == "Beat Div")
	{
		ofxUISlider *slider = (ofxUISlider *) e.widget; 
		beatDiv = int(slider->getScaledValue()); 
	}
	
	
	else if(name =="Waveform")
	{
		ofxUIDropDownList* wf = (ofxUIDropDownList *) e.widget;
		
		vector<ofxUIWidget *> &selected = wf->getSelected(); 
        string selectedName;
		for(int i = 0; i < selected.size(); i++)
        {
            selectedName=selected[i]->getName();
			wf->setLabelText(selectedName);
			if(selectedName=="Sin")
			{
				selectedWaveform=0;
			}
			else if(selectedName=="Cos")
			{
				selectedWaveform=1;
			}
			else if(selectedName=="Square")
			{
				selectedWaveform=2;
			}
			else if(selectedName=="Saw")
			{
				selectedWaveform=3;
			}
			else if(selectedName=="Triangle")
			{
				selectedWaveform=4;
			}
			else if(selectedName=="Random")
			{
				selectedWaveform=5;
				//ofSeedRandom();
			}
			
        }
		printf("selected waveform %d\n",selectedWaveform);
		
	}
//	else if(name =="Resync")
//	{
//		n=0;
//		phasor=0;
//	}	
	else if(name =="Invert")
	{
		ofxUIToggle *t = (ofxUIToggle *) e.widget; 
		if(t->getValue()) invertToggle = true;
		else invertToggle = false;
	}
	else if(name =="Beats / Floats")
	{
		ofxUIToggle *t = (ofxUIToggle *) e.widget; 
		if(t->getValue()) beatsFloats = true;
		else beatsFloats = false;
	}
	
	
	
	

	else if(name =="Quantize")
	{
		ofxUISlider *s = (ofxUISlider *) e.widget; 
		quantize = s->getScaledValue();
	}	
	else if(name =="Power")
	{
		ofxUISlider *s = (ofxUISlider *) e.widget; 
		power = s->getScaledValue();
	}	
	else if(name =="min")
	{
		ofxUINumberDialer *s = (ofxUINumberDialer *) e.widget; 
		ampMin = double(s->getValue());
	}	
	else if(name =="max")
	{
		ofxUINumberDialer *s = (ofxUINumberDialer *) e.widget; 
		ampMax = double(s->getValue());	
	}	
	
	
	
	
}


//--------------------------------------------------------------
void pmLFO::setBPM(double _bpm)
{
	bpm = _bpm;
}



		
//--------------------------------------------------------------
void pmLFO::loadPreset(int i)
{
	string s = "GUI/presets/" +name +ofToString(i) +".xml";
	
	gui->loadSettings(s);
	
	ofxUIDropDownList* wf = (ofxUIDropDownList *) gui->getWidget("Waveform"); 
	vector<ofxUIWidget *> &selected = wf->getSelected(); 
	string selectedName;
	
	printf("loading preset selected waveforms %d\n",selected.size());
	
	for(int i = 0; i < selected.size(); i++)
	{
		selectedName=selected[i]->getName();		
		wf->setLabelText(selectedName);
		if(selectedName=="Sin")
		{
			selectedWaveform=0;
		}
		else if(selectedName=="Cos")
		{
			selectedWaveform=1;
		}
		else if(selectedName=="Square")
		{
			selectedWaveform=2;
		}
		else if(selectedName=="Saw")
		{
			selectedWaveform=3;
		}
		else if(selectedName=="Triangle")
		{
			selectedWaveform=4;
		}
		else if(selectedName=="Random")
		{
			selectedWaveform=5;
		}
		
	}

	resync();

}

//--------------------------------------------------------------
void pmLFO::savePreset(int i)
{	
	gui->saveSettings("GUI/presets/" +name +ofToString(i) +".xml");
}


//--------------------------------------------------------------
void pmLFO::setOscIP(string s)
{
	oscIP = s;
	pmOscSender.setup(oscIP,12345);
}

