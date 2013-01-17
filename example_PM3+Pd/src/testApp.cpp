#include "testApp.h"
using namespace ofxPm;

//
// check this ofxFastFboReader !!
//

bool drawHelp = false;

//--------------------------------------------------------------
void testApp::setup(){
	
	patchName = "basic.pd";
	durationInFrames = 400;
	showGUI = true;
	fullscreen = false;
	drawAudioResolution=1;	
	grabberResolution = ofVec2f(640.0,480.0);
	grabberAspectRatio = grabberResolution.x / grabberResolution.y;

	////////////////////
	// ofxGUI
	////////////////////	
	gui.setup();
	
	gui.setName("PLAYMODES");
	gui.add(setupLabel.setup			(ofToString("setupLabel"),ofToString("setup : ")+ofToString(patchName)));
	gui.add(fps.setup					("rec at fps",30,1,200));
	gui.add(avSyncOffset.setup			("av-sync offset",0,-50,50));
	gui.add(opacityIn.setup				("opacity in",1.0,0.0,1.0));
	gui.add(opacityOut.setup			("opacity out",1.0,0.0,1.0));
	gui.add(dryWet.setup				("dry / wet",1.0,0.0,1.0));
	gui.add(drawAudioToggle.setup		("draw audio",true));	
	gui.add(grabberSettingsToggle.setup	("grabber settings",false));
	gui.add(opacityEnvToggle.setup		("opacity env",false));

	gui.add(controlLabel.setup		(ofToString("controlLabel"),ofToString("control")));
	gui.add(rec.setup				("rec",true));
	gui.add(play.setup				("play",false));
	gui.add(delay.setup				("delay ms",0.0,0.0,13333.0));
	gui.add(length.setup			("length ms",13333.0,0.0,13333.0));
	gui.add(in.setup				("in ms",13333.0,0.0,13333.0));
	gui.add(out.setup				("out ms",0.0,0.0,13333.0));
	gui.add(speed.setup				("speed",1.0,-4.0,4.0));
	gui.add(feedBack.setup			("feedBack",0.0,0.0,1.0));

	gui.add(audioLabel.setup		(ofToString("audioLabel"),ofToString("audio")));
	gui.add(scratchFactor.setup		("scratch factor",0.0,0.0,4000.0));
	gui.add(attack.setup			("attack",0.0,0.0,2000.0));
	gui.add(decay.setup				("decay",0.0,0.0,2000.0));
	gui.add(fullGrain.setup			("play full grain",false));
	gui.add(presetLabel.setup		(ofToString("presetLabel"),ofToString("preset : none")));
	
	////////////////////
	// - add listener's for events.	
	////////////////////
	gui.getFloatSlider("in ms").addListener(this,&testApp::inChanged);
	gui.getFloatSlider("out ms").addListener(this,&testApp::outChanged);
	gui.getFloatSlider("length ms").addListener(this,&testApp::lengthChanged);
	gui.getFloatSlider("delay ms").addListener(this,&testApp::delayChanged);
	gui.getFloatSlider("speed").addListener(this,&testApp::speedChanged);
	gui.getFloatSlider("feedBack").addListener(this,&testApp::feedBackChanged);
	gui.getFloatSlider("opacity in").addListener(this,&testApp::opacityInChanged);
	gui.getFloatSlider("opacity out").addListener(this,&testApp::opacityOutChanged);
	gui.getFloatSlider("attack").addListener(this,&testApp::attackChanged);
	gui.getFloatSlider("decay").addListener(this,&testApp::decayChanged);
	gui.getFloatSlider("dry / wet").addListener(this,&testApp::dryWetChanged);
	
	gui.getIntSlider("rec at fps").addListener(this,&testApp::fpsChanged);
	gui.getIntSlider("av-sync offset").addListener(this,&testApp::avSyncOffsetChanged);
	gui.getIntSlider("scratch factor").addListener(this,&testApp::scratchFactorChanged);	

	gui.getToggle("rec").addListener(this,&testApp::recChanged);	
	gui.getToggle("play").addListener(this,&testApp::playChanged);	
	gui.getToggle("draw audio").addListener(this,&testApp::drawAudioChanged);	
	gui.getToggle("play full grain").addListener(this,&testApp::fullGrainChanged);	
	gui.getToggle("opacity env").addListener(this,&testApp::opacityEnvToggleChanged);	
	gui.getToggle("grabber settings").addListener(this,&testApp::grabberSettingsToggleChanged);	
	
	////////////////////
	// fbo and pixel allocation
	////////////////////
	fboA.allocate(grabberResolution.x, grabberResolution.y,GL_RGB,0);
	fboFeedback.allocate(grabberResolution.x, grabberResolution.y,GL_RGB,0);
	pixelsToBuffer.allocate(grabberResolution.x, grabberResolution.y,OF_PIXELS_RGB);

	////////////////////
	// playmodes3 pipeline basic init
	////////////////////
	grabber.initGrabber(grabberResolution.x,grabberResolution.y);
	vRate.setup(grabber,fps);
	vBuffer.setup(vRate,durationInFrames);
	vHeader.setup(vBuffer);
	vHeader.setDelayMs(delay);
	vHeader.setInMs(in);
	vHeader.setOutMs(out);
	vHeader.setLengthMs(13333.0);	
	vHeader.setWindowPriority("length");
	vBuffer.setInSource(vHeader);
	vRendererBuffer.setup(vRate);
	vRendererHeader.setup(vHeader);
	ofEnableAlphaBlending();

	
	////////////////////
	// pd stuff
	////////////////////
	indexArrayFromPd=0;	
	sampleRate = 96000;
	// puredata works on sounds in chunks of 64 samples (called a tick)
	// 8 ticks per buffer corresponds to 8 * 64 sample buffer size (512 samples)
	int ticksPerBuffer = 8;
	
	// this initialized pd
	// format is pd.init(numOutputChannels, numInputChannels, samplerate, ticksPerBuffer)
	// note that outputs comes before inputs.
	pd.init(2, 2, sampleRate, ticksPerBuffer);
	
	// load libs
	expr_setup();
	zexy_setup();
	susloop_tilde_setup();
	
	// open the patch (relative to the data folder)	
	pd.openPatch(patchName);
	
	// start pd
	pd.computeAudio(true);
	
	pd.subscribe("delayDoppler");
	pd.subscribe("opacityEnv");
	pd.subscribe("loopSync");
	pd.subscribe("index");
	pd.addReceiver(*this);
		
	// start the sound stream - always do this last in your setup
	ofSoundStreamSetup(2, 2, this, sampleRate, 64*ticksPerBuffer, 1);
	
	// init pd object
	pd.sendFloat("sampleRate",float(sampleRate));
	pd.sendFloat("frameRate",30.0f);
	pd.sendFloat("bufferSize",sampleRate * (float(durationInFrames) / fps));
	pd.sendFloat("speed",1.0f);
	pd.sendFloat("doppler",0.0f);
	pd.sendFloat("delay",0.0f);
	pd.sendFloat("feedback",0.0f);
	pd.sendFloat("opacityOut",1.0f);
	pd.sendFloat("in",13333.333);
	pd.sendFloat("length",13333.333);
	pd.sendFloat("fullGrain",0.0f);
	pd.sendFloat("attack",0.0f);
	pd.sendFloat("decay",0.0f);
	pd.sendFloat("dryWet",1.0f);
	pd.sendFloat("opacityEnvToggle",0.0f);
	pd.sendFloat("out",0.0f);
	pd.sendFloat("play",0.0f);
	pd.sendFloat("rec",1.0f);

	////////////////////////
	// other stuff 
	////////////////////////
	
	//ofSetVerticalSync(true);
	ofBackground(25);
	
}


//--------------------------------------------------------------
void testApp::update()
{
	ofBackground(25);

	// ready array from Pd
	pd.readArray("playmodes", arrayFromPd);
	
	if(videoGrabber.isLoaded()) videoGrabber.update();
	else grabber.update();


}

//--------------------------------------------------------------
void testApp::draw(){
	
	//- FBO FeedBack 
	// first we draw/calculate the feedback loop of the pipeline.
	// the idea is to mix the rec-buffer image with the one from the header 
	// which is multiplied by the feedback factor and put again into the buffer
	ofBackground(0,0,0);
	ofPushMatrix();
	fboFeedback.begin();
	// first draw the background
	ofSetColor(0,255);
	ofRect(0,0,grabberResolution.x,grabberResolution.y);
	ofEnableAlphaBlending();
	ofSetColor(255*opacityIn,255*opacityIn);
	// draw the buffer on the base
	vRendererBuffer.draw();
	glBlendEquationEXT(GL_MAX_EXT);
	ofSetColor(254.0*log(feedBack),254.0*log(feedBack));
	// and the header on top
	vRendererHeader.draw();
	glBlendEquationEXT(GL_FUNC_ADD_EXT);
	ofDisableAlphaBlending();
	fboFeedback.end();
	ofPopMatrix();
	
	// read pixels from fbo and put them into the buffer
	fboFeedback.readToPixels(pixelsToBuffer);
	vBuffer.setPixelsIn(pixelsToBuffer);
	
	//- FBO dryWet 
	// now we draw the drywet part of the pipeline.
	// the idea is to mix in a full-power cross fade curve the buffer and the header again
	// but without "recording" the result on the buffer.
	
	ofPushMatrix();
	fboA.begin();
	// draw the background in black
	ofSetColor(0,255);
	ofRect(0,0,grabberResolution.x,grabberResolution.y);	

	// dry - wet values
	float dwA;
	float dwB;
	if(dryWet<0.5f)
	{
		dwA=dryWet*2.0;
		dwB=1.0;
	}
	else 
	{
		dwB=(dryWet-1.0f)*-2.0;
		dwA=1.0;
	}	
	// for the wet drt we use ADD
	ofEnableBlendMode(OF_BLENDMODE_ADD);
	ofSetColor(255,255*dwB);
	// draw the buffer 
	vRendererBuffer.draw();
	ofSetColor(254,254*(dwA));
	// draw the header
	vRendererHeader.draw();
	ofDisableBlendMode();
	fboA.end();
	ofPopMatrix();

	//glBlendEquationEXT(GL_FUNC_ADD_EXT);
	//ofSetColor(254.0*log(feedBack),254.0*log(feedBack));
	

	if(!fullscreen)
	{
		ofSetColor(255);
		ofPushMatrix();
		ofTranslate(300,20);
		float resizeFactorX = (ofGetWidth()-320.0f-20.0f)/3.0f;
		float resizeFactorY = ((ofGetWidth()-320.0f-20.0f)/grabberAspectRatio)/3.0f;
		vRendererBuffer.draw(0,0,resizeFactorX,resizeFactorY);
		ofPopMatrix();
		ofPushMatrix();
		ofTranslate(320.0+resizeFactorX,20.0f);
		//ofSetColor(vHeader.getOpacity(),vHeader.getOpacity(),vHeader.getOpacity());
		//vRendererHeader.draw(0,0,resizeFactorX*2.0f,resizeFactorY*2.0f);
		ofSetColor(opacityOut*255);
		fboA.draw(0,0,resizeFactorX*2.0f,resizeFactorY*2.0f);
		ofPopMatrix();
	}
	else 
	{
		ofBackground(0);
		ofSetColor(255);
		float screenAspectRatio = float(ofGetWidth()) / float(ofGetHeight());
		if (screenAspectRatio>grabberAspectRatio) 
		{
			vRendererHeader.draw((ofGetWidth()-ofGetHeight()*grabberAspectRatio)/2,0,ofGetHeight()*grabberAspectRatio,ofGetHeight());
		}
		else 
		{
			vRendererHeader.draw(0,0,ofGetWidth(),ofGetWidth()/grabberAspectRatio);
		}
	}
	if(showGUI)
	{
		vBuffer.draw();
		vHeader.draw();
		if(drawAudioToggle) drawPdBuffer();
		gui.draw();
		if(pd.isInited())
			ofSetColor(128);
		else 
			ofSetColor(128,0,0);
		
		ofDrawBitmapString(patchName + "  ||  " + ofToString(int(ofGetFrameRate()))
						   + " fps "
						   + " || recRate " + ofToString(vBuffer.getRealFPS()) + " fps"
						   ,20,ofGetHeight()-30);
		ofDrawBitmapString(vHeader.getInfo(),20,ofGetHeight()-10);
		
	}
	
	// draw thumbnails of the graphic flow
	if(drawHelp)
	{
		
		ofSetColor(255);
		ofPushMatrix();
		ofTranslate(20, 500,0);
		ofScale(0.25,0.25,0.25);
		vRendererBuffer.draw();
		ofTranslate(640,0,0);
		vRendererHeader.draw();
		fboFeedback.draw(640,0);
		fboA.draw(1280,0);
		ofPopMatrix();
		ofSetColor(0,255,0);
		ofDrawBitmapString("grabber",20,480);
		ofDrawBitmapString("header",20+160,480);
		ofDrawBitmapString("fboFeedBack",20+160*2,480);
		ofDrawBitmapString("fboDryWet",20+160*3,480);
	}
	
	//	ofSetColor(0,0,128);
	//	ofCircle(PMDRAWSPACING + ((ofGetWidth()-PMDRAWSPACING)*(float(indexArrayFromPd)/float(1280000))),ofGetHeight(),20);
	
}
//--------------------------------------------------------------
void testApp::keyPressed(int key){

	printf("key %d\n",key);
	if(key=='s') grabber.videoSettings();
	else if(key=='f') 
	{
		fullscreen=!fullscreen;
		ofToggleFullscreen();
		if(!fullscreen) showGUI=true;
		else showGUI=false;
	}
	else if(key=='h') 
	{
		if (fullscreen) showGUI=!showGUI;
	}
	else if(key==OF_KEY_F1) 
	{
		drawHelp=!drawHelp;
	}
	else if (key=='r')
	{
		vHeader.setLoopToStart();
	}
	// PRESETs 
	///////////////
	else if (key=='1')
	{
		gui.loadFromFile("1.xml");
		presetLabel = ofToString("preset : 1.xml");
	}
	else if (key=='!')
	{
		gui.saveToFile("1.xml");
	}
	else if (key=='2')
	{
		gui.loadFromFile("2.xml");
		presetLabel = ofToString("preset : 2.xml");
	}
	else if (key=='"')
	{
		gui.saveToFile("2.xml");
	}
	else if (key=='3')
	{
		gui.loadFromFile("3.xml");
		presetLabel = ofToString("preset : 3.xml");
	}
	else if (key==183)
	{
		gui.saveToFile("3.xml");
	}
	else if (key=='4')
	{
		gui.loadFromFile("4.xml");
		presetLabel = ofToString("preset : 4.xml");
	}
	else if (key=='$')
	{
		gui.saveToFile("4.xml");
	}
	else if (key=='5')
	{
		gui.loadFromFile("5.xml");
		presetLabel = ofToString("preset : 5.xml");
	}
	else if (key=='%')
	{
		gui.saveToFile("5.xml");
	}
	else if (key=='6')
	{
		gui.loadFromFile("6.xml");
		presetLabel = ofToString("preset : 6.xml");
	}
	else if (key=='&')
	{
		gui.saveToFile("6.xml");
	}
	else if (key=='7')
	{
		gui.loadFromFile("7.xml");
		presetLabel = ofToString("preset : 7.xml");
	}
	else if (key=='/')
	{
		gui.saveToFile("7.xml");
	}
	else if (key=='8')
	{
		gui.loadFromFile("8.xml");
		presetLabel = ofToString("preset : 8.xml");
	}
	else if (key=='(')
	{
		gui.saveToFile("8.xml");
	}
	else if (key=='9')
	{
		gui.loadFromFile("9.xml");
		presetLabel = ofToString("preset : 9.xml");
	}
	else if (key==')')
	{
		gui.saveToFile("9.xml");
	}
	else if (key=='0')
	{
		gui.loadFromFile("0.xml");
		presetLabel = ofToString("preset : 0.xml");
	}
	else if (key==')')
	{
		gui.saveToFile("0.xml");
	}
	
	
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

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
	
	
	
	videoGrabber.loadMovie(dragInfo.files[0]);
	durationInFrames = videoGrabber.getTotalNumFrames();
	grabberResolution = ofVec2f(videoGrabber.getWidth(),videoGrabber.getHeight());
	grabberAspectRatio = grabberResolution.x / grabberResolution.y;
	fboA.allocate(grabberResolution.x, grabberResolution.y,GL_RGB,0);
	fboFeedback.allocate(grabberResolution.x, grabberResolution.y,GL_RGB,0);
	pixelsToBuffer.allocate(grabberResolution.x, grabberResolution.y,OF_PIXELS_RGB);
		
	vHeader.setDelayMs(0.0f);
	vHeader.setLengthMs(videoGrabber.getDuration()*1000.0);
	vHeader.setInMs(videoGrabber.getDuration()*1000.0);
	//	vHeader.setOutMs(out);
	vHeader.setWindowPriority("length");
	vBuffer.stop();
	vRate.removeListener();
	
	vBuffer.setup(videoGrabber,durationInFrames);
	vBuffer.resume();
	vBuffer.setInSource(vHeader);
	vHeader.setup(vBuffer);
	vRendererBuffer.setup(videoGrabber);
	vRendererHeader.setup(vHeader);
	
	
	
	videoGrabber.play();

	//vBuffer.setPixelsIn(pixelsToBuffer);

	printf("dragEvent : file %s [%d frames] [duration %f]\n",dragInfo.files[0].c_str(),durationInFrames,videoGrabber.getDuration()*1000.0);
	pd.sendSymbol("videoFileName",dragInfo.files[0]);


}

// this is where the openframeworks sound stream connects to ofxPd
// it's also where the audio processing is done
//--------------------------------------------------------------
void testApp::audioOut(float *output, int bufferSize, int numChannels) {
	pd.audioOut(output, bufferSize, numChannels);
}
// this is how you get audio into pd
void testApp::audioIn(float *input, int bufferSize, int numChannels) {
	pd.audioIn(input, bufferSize, numChannels);
}



//--------------------------------------------------------------
void testApp::receiveFloat(const std::string& dest, float value)
{	
	float dryWetAlpha = 0.09;
	if(dest=="delayDoppler") 
	{
		vHeader.setDelayMs(value/(sampleRate/1000.0f));
		printf("received from pd> delayDoppler!!\n");		
	}
	else if(dest=="opacityEnv") 
	{
//		if(opacityEnvToggle) opacityIn=value*10.0;
		if(opacityEnvToggle) dryWet=((1.0f-dryWetAlpha)*dryWet)+(dryWetAlpha*value*10.0);
		if(dryWet>1.0) dryWet=1.0;
		printf("received from pd> opacityOut!!\n");		
	}
	else if(dest=="loopSync")
	{
		vHeader.setLoopToStart();
		printf("received from pd> loopSync!!\n");
	}
	else if(dest=="index")
	{
		/*if(!play)*/ indexArrayFromPd = int(value);
	}
}

//--------------------------------------------------------------
void testApp::drawPdBuffer()
{

	int drawBufferResolution = 10;
	
	if((arrayFromPd.size()>1)&&(rec))
	{	
		arrayForDrawAudio.clear();

		/*
		int delayInSamples = (delay / 1000.0f) * float(sampleRate);
		int ii,sampledIndex,sampleToRead;
		float factor,clampedValue;
		for(int i=0;i<ofGetWidth()-(2*PMDRAWSPACING);i=i+2)
		{
			factor = float(i)/float(ofGetWidth()-(2*PMDRAWSPACING));
			sampledIndex = 	int(float(arrayFromPd.size()) * factor);
			sampleToRead = indexArrayFromPd - sampledIndex - delayInSamples;
			if (sampleToRead <0) sampleToRead = sampleToRead + arrayFromPd.size();
			clampedValue = CLAMP(arrayFromPd[sampleToRead],-1.0,1.0);
			ofLine(ofGetWidth()-PMDRAWSPACING-float(i),ofGetHeight()-180,ofGetWidth()-PMDRAWSPACING-float(i),ofGetHeight()-180+(40.0f*clampedValue));
		}
		*/
		
		int pixelsToDraw = (ofGetWidth()-(2*PMDRAWSPACING));
		int	samplesPerPixel = arrayFromPd.size() / pixelsToDraw;

		float min=0.0f;
		float max=0.0f;
		float value=0.0f;
		int ii;
		
		for(int i=0;i<ofGetWidth()-(2*PMDRAWSPACING);i=i+drawAudioResolution)
		{
			min=0.0f;
			max=0.0f;
			value=0.0f;
			ii = i*samplesPerPixel;
			int newIndex;
			for(int j=0;j<samplesPerPixel;j++)
			{
				newIndex = (indexArrayFromPd) - (ii-j);
				if(newIndex<0) newIndex=arrayFromPd.size()+newIndex;
				value = arrayFromPd[newIndex];
				
				if(value<min) 
				{
					min=value;
					if(min<-1.0) min=-1.0;
				}
				else if(value>max)
				{
					max=value;
					if(max>1.0) max=1.0;
					
				}
			}
			ofVec2f minMax;
			minMax.set(min,max);
			arrayForDrawAudio.push_back(minMax);
			
		}
	}

	// draw the wav
	ofEnableAlphaBlending();
	ofSetColor(0,150,255,200);
	
	for(int i=0;i<ofGetWidth()-(2*PMDRAWSPACING);i=i+drawAudioResolution)
	{
		ofLine(ofGetWidth()-PMDRAWSPACING-(i),ofGetHeight()-180+arrayForDrawAudio[i/drawAudioResolution].x*40,ofGetWidth()-PMDRAWSPACING-(i),ofGetHeight()-180+arrayForDrawAudio[i/drawAudioResolution].y*40);	
	}
	ofDisableAlphaBlending();
	
}



// float change's
//--------------------------------------------------------------
void testApp::inChanged(float &f)
{
	vHeader.setInMs(f);
	pd.sendFloat("in",vHeader.getIn());
	printf("> in changed to %f\n",f);
}
//--------------------------------------------------------------
void testApp::outChanged(float &f)
{
	vHeader.setOutMs(f);
	pd.sendFloat("out",vHeader.getOut());
	printf("> out changed to %f\n",f);

}
//--------------------------------------------------------------
void testApp::lengthChanged(float &f)
{
	vHeader.setLengthMs(f);
	pd.sendFloat("length",vHeader.getLength());
	printf("> length changed to %f\n",f);
}
//--------------------------------------------------------------
void testApp::delayChanged(float &f)
{
	pd.sendFloat("delay",f);
	printf("> delay changed to %f\n",f);
	
}
//--------------------------------------------------------------
void testApp::speedChanged(float &f)
{
	vHeader.setSpeed(f);
	pd.sendFloat("speed",f);
	printf("> speed changed to %f\n",f);
}
//--------------------------------------------------------------
void testApp::feedBackChanged(float &f)
{
	pd.sendFloat("feedBack",f);
	vBuffer.setFeedBack(f);
	printf("> feedback changed to %f\n",f);
}
//--------------------------------------------------------------
void testApp::opacityOutChanged(float &f)
{
	vHeader.setOpacity(int(f*255.0));
	pd.sendFloat("opacityOut",f);
	printf("> opacity out changed to %f\n",f);
}
//--------------------------------------------------------------
void testApp::opacityInChanged(float &f)
{
//	if(opacityEnvToggle)
//	{
//		printf("> opacity in changes from pd to %f because toggle is on\n",f);
//	}
//	else {
//		vHeader.setOpacity(int(f*255.0));
		printf("> opacity in changed to %f\n",f);
//	}
	
}
//--------------------------------------------------------------
void testApp::attackChanged(float &f)
{
	pd.sendFloat("attack",f);
	printf(">attack changed to %f\n",f);
}
//--------------------------------------------------------------
void testApp::decayChanged(float &f)
{
	pd.sendFloat("decay",f);
	printf(">decay changed to %f\n",f);
}
//--------------------------------------------------------------
void testApp::dryWetChanged(float &f)
{
	pd.sendFloat("dryWet",f);
	printf(">dryWet changed to %f\n",f);
}


//--------------------------------------------------------------
// int change's
//--------------------------------------------------------------
void testApp::fpsChanged(int &i)
{
	vRate.setFps(i);
	printf(">fps changed to %d\n",i);

}
//--------------------------------------------------------------
void testApp::avSyncOffsetChanged(int &i)
{
	vHeader.setOffsetInFrames(i);
	printf(">avSync offset changed to %d\n",i);
}
//--------------------------------------------------------------
void testApp::scratchFactorChanged(int &i)
{
	pd.sendFloat("doppler",i);
	printf(">scratch factor changed to %d\n",i);
}

//--------------------------------------------------------------
// bool change's
//--------------------------------------------------------------
void testApp::recChanged(bool &b)
{
	if(!b)
	{
		vBuffer.stop();
		pd.sendFloat("rec",0.0);
		printf("> rec stop 0.0\n");
	}
	else
	{
		vBuffer.resume();
		pd.sendFloat("rec",1.0);
		printf("> rec play changed to 1.0\n");
	}
	
	
}
//--------------------------------------------------------------
void testApp::playChanged(bool &b)
{
	if(b) {
		vHeader.setPlaying(true);
		pd.sendFloat("play",1.0);
		printf("> play changed to 1.0\n");
		
	}
	else 
	{
		vHeader.setPlaying(false);
		vHeader.setDelayMs(delay);
		pd.sendFloat("play",0.0);
		printf("> play changed to 0.0\n");
	}	
}
//--------------------------------------------------------------
void testApp::drawAudioChanged(bool &b)
{
}
//--------------------------------------------------------------
void testApp::fullGrainChanged(bool &b)
{
	if(fullGrain)
	{
		pd.sendFloat("fullGrain",1.0f);
		printf("> full grain changed to 1.0\n");
	}
	else 
	{
		pd.sendFloat("fullGrain",0.0f);
		printf("> full grain changed to 0.0\n");
	}
}
//-------------------------------------------------------------
void testApp::opacityEnvToggleChanged(bool &b)
{
	
	if(!opacityEnvToggle)
	{
		pd.sendFloat("opacityEnvToggle",1.0f);
		printf("> oppacity Env Toggle changed to 1.0\n");
	}
	else 
	{
		pd.sendFloat("opacityEnvToggle",0.0f);
		//pd.sendFloat("opacityOut",opacityOut);
		printf("> oppacity Env Toggle changed to 0.0\n");
	}
	
}

//-------------------------------------------------------------
void testApp::grabberSettingsToggleChanged(bool &b)
{
	if(b)
	{
		grabber.videoSettings();
		b = grabberSettingsToggle =false;
		printf("> grabber video settings \n");
	}
	

}

