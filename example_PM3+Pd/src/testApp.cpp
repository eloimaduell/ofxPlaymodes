#include "testApp.h"
using namespace ofxPm;

//--------------------------------------------------------------
void testApp::setup(){
	
	showGUI = true;
	fullscreen = false;
	drawAudioResolution=1;
	
	gui.setup();
	gui.setName("b a s i c");
	gui.add(fps.setup("rec Rate fps",30,1,200));
	gui.add(delay.setup("vH delay ms",0.0,0.0,13333.0));
	gui.add(speed.setup("vH speed",1.0,-4.0,4.0));
	gui.add(in.setup("vH in ms",13333.0,0.0,13333.0));
	gui.add(out.setup("vH out ms",0.0,0.0,13333.0));
	gui.add(length.setup("vH length ms",13333.0,0.0,13333.0));
	gui.add(feedBack.setup("feedBack",0.0,0.0,1.0));
	gui.add(opacityOut.setup("opacity out",1.0,0.0,1.0));
	gui.add(opacityEnvToggle.setup("opacity Env",false));
	gui.add(fullGrain.setup("play full grain",false));
	gui.add(doppler.setup("scratch factor",0.0,0.0,4000.0));
	gui.add(drawAudioToggle.setup("draw audio",true));	
	//gui.add(drawAudioResolution.setup("draw audio res",1,1,15));
	gui.add(rec.setup("rec",true));
	gui.add(play.setup("play",false));
	gui.add(avSyncOffset.setup("av sync offset",0,-50,50));
	
	
	oldSpeed			= 1.0;
	oldDelay			= 0.0;
	oldDoppler			= 0.0;
	oldRec				= 1.0;
	oldPlay				= 1.0;
	oldOpacityOut		= 1.0;
	oldLength			= 13333.0;
	oldFullGrain		= false;
	oldOpacityEnvToggle = false;
	
			
	grabber.initGrabber(640,480);
	vRate.setup(grabber,fps);
	vBuffer.setup(vRate,400);
	vHeader.setup(vBuffer);
	vHeader.setDelayMs(delay);
	vHeader.setInMs(in);
	vHeader.setOutMs(out);
	vHeader.setLengthMs(13333.0);	
	vHeader.setWindowPriority("length");

	vBuffer.setInSource(vHeader);

	vRenderer1.setup(vRate);
	vRenderer2.setup(vHeader);
	ofEnableAlphaBlending();
	//ofSetVerticalSync(true);
	ofBackground(25);
	
	grabberResolution = ofVec2f(640.0,480.0);
	grabberAspectRatio = grabberResolution.x / grabberResolution.y;
	
	
	// pd stuff
	////////////
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
	patchName = "basic.pd";
	//patchName = "santi.pd";
	//patchName = "testExpr.pd";
	
	pd.openPatch(patchName);
	//pd.openPatch("theremin.pd");
	//pd.openPatch("echochamber.pd");
	
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
	pd.sendFloat("bufferSize",sampleRate * (400.0 / fps));
	pd.sendFloat("speed",1.0f);
	pd.sendFloat("doppler",0.0f);
	pd.sendFloat("delay",0.0f);
	pd.sendFloat("opacityOut",1.0f);
	pd.sendFloat("in",13333.333);
	pd.sendFloat("length",13333.333);
	pd.sendFloat("fullGrain",0.0f);
	pd.sendFloat("opacityEnvToggle",0.0f);
	pd.sendFloat("out",0.0f);
	pd.sendFloat("play",0.0f);
	pd.sendFloat("rec",1.0f);
	
}


//--------------------------------------------------------------
void testApp::update(){
	
	// to do : pass all ofxGui changes with events ...
	
	grabber.update();

	vBuffer.setFeedBack(feedBack);
	
	vRate.setFps(fps);
	vHeader.setFps(fps);
	// delay is off because it's driven by pd receiver
	//vHeader.setDelayMs(delay);
	vHeader.setInMs(in);
	vHeader.setOutMs(out);
	vHeader.setOffsetInFrames(avSyncOffset);
	vHeader.setOpacity(int(opacityOut*255.0));
	

	if(!rec && (oldRec!=0.0))
	{
		vBuffer.stop();
		pd.sendFloat("rec",0.0);
		oldRec=0.0f;
		printf("send rec 0\n");
	}
	else if(rec && (oldRec!=1.0))
	{
		vBuffer.resume();
		pd.sendFloat("rec",1.0);
		oldRec=1.0f;
		printf("send rec 1\n");
	}
	
	if(play && !vHeader.isPlaying()) {
		vHeader.setPlaying(true);
		pd.sendFloat("play",1.0);
		printf("play 1\n");
	}
	else if (!play && vHeader.isPlaying()) 
	{
		vHeader.setPlaying(false);
		vHeader.setDelayMs(delay);
		pd.sendFloat("play",0.0);
		printf("play 0\n");
	}
	
	// PD STUFF
	if(oldSpeed!=float(speed.value))
	{
		vHeader.setSpeed(speed);
		pd.sendFloat("speed",speed);
		oldSpeed=speed.value;
		printf("send speed %f oldSpeed %f\n",float(speed.value),oldSpeed);
	}
	if(oldDoppler!=float(doppler))
	{
		pd.sendFloat("doppler",doppler);
		oldDoppler=float(doppler);
		printf("send doppler %f\n",float(doppler));
	}
	if(oldDelay!=float(delay))
	{
		pd.sendFloat("delay",delay);
		oldDelay=float(delay);
		printf("send delay %f\n",float(delay));
	}
	if(oldOpacityOut!=float(opacityOut))
	{
		// if opacityEnvToggle is on means we're receiving opacityOut values, so no need to send it back to pd !
		if(!opacityEnvToggle) pd.sendFloat("opacityOut",opacityOut);
		oldOpacityOut=float(opacityOut);
		printf("send opacityOut %f\n",float(opacityOut));
	}
	if(oldLength!=float(length.value))
	{
		pd.sendFloat("length",length);
		oldLength=length.value;
		printf("send length %f length %f\n",float(length.value),oldLength);
		vHeader.setLengthMs(length);
	}
	if(oldFullGrain!=fullGrain)
	{
		if(fullGrain)
		{
			pd.sendFloat("fullGrain",1.0f);
			printf("send fullGrain %f\n",1.0f);
		}
		else 
		{
			printf("send fullGrain %f\n",0.0f);
			pd.sendFloat("fullGrain",0.0f);
		}
		oldFullGrain=fullGrain;
	}
	if(oldOpacityEnvToggle!=opacityEnvToggle)
	{
		if(opacityEnvToggle)
		{
			pd.sendFloat("opacityEnvToggle",1.0f);
			printf("send opacityEnvToggle %f\n",1.0f);
		}
		else 
		{
			printf("send opacityEnvToggle %f\n",0.0f);
			pd.sendFloat("opacityEnvToggle",0.0f);
			opacityOut = 1.0f;
		}
		oldOpacityEnvToggle=opacityEnvToggle;
	}

	pd.sendFloat("in",vHeader.getIn());
	pd.sendFloat("out",vHeader.getOut());
	pd.sendFloat("length",vHeader.getLength());
	
	// ready array
	pd.readArray("playmodes", arrayFromPd);

}

//--------------------------------------------------------------
void testApp::draw(){
	
	if(!fullscreen)
	{
		ofBackground(25);
		ofSetColor(255);
		ofPushMatrix();
		ofTranslate(300,20);
		float resizeFactorX = (ofGetWidth()-320.0f-20.0f)/3.0f;
		float resizeFactorY = ((ofGetWidth()-320.0f-20.0f)/grabberAspectRatio)/3.0f;
		vRenderer1.draw(0,0,resizeFactorX,resizeFactorY);
		ofPopMatrix();
		ofPushMatrix();
		ofTranslate(320.0+resizeFactorX,20.0f);
		ofSetColor(vHeader.getOpacity(),vHeader.getOpacity(),vHeader.getOpacity());
		vRenderer2.draw(0,0,resizeFactorX*2.0f,resizeFactorY*2.0f);
		ofPopMatrix();

		

	}
	else 
	{
		ofBackground(0);
		ofSetColor(255);
		float screenAspectRatio = float(ofGetWidth()) / float(ofGetHeight());
		if (screenAspectRatio>grabberAspectRatio) 
		{
			vRenderer2.draw((ofGetWidth()-ofGetHeight()*grabberAspectRatio)/2,0,ofGetHeight()*grabberAspectRatio,ofGetHeight());
		}
		else 
		{
			vRenderer2.draw(0,0,ofGetWidth(),ofGetWidth()/grabberAspectRatio);
		}

	}
	
	
	if(showGUI)
	{
		vBuffer.draw();
		vHeader.draw();

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
	
	if(drawAudioToggle) drawPdBuffer();
	
	ofSetColor(0,0,128);
	ofCircle(PMDRAWSPACING + ((ofGetWidth()-PMDRAWSPACING)*(float(indexArrayFromPd)/float(1280000))),ofGetHeight(),20);
}
//--------------------------------------------------------------
void testApp::keyPressed(int key){

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
	else if (key=='r')
	{
		vHeader.setLoopToStart();
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
	if(dest=="delayDoppler") 
	{
		vHeader.setDelayMs(value/(sampleRate/1000.0f));
	}
	else if(dest=="opacityEnv") 
	{
		if(opacityEnvToggle) opacityOut=value;
	}
	else if(dest=="loopSync")
	{
		vHeader.setLoopToStart();
		printf("loopSync!!\n");
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
	ofSetColor(0,150,255);
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
	ofSetColor(0,150,255);
	for(int i=0;i<ofGetWidth()-(2*PMDRAWSPACING);i=i+drawAudioResolution)
	{
		ofLine(ofGetWidth()-PMDRAWSPACING-(i),ofGetHeight()-180+arrayForDrawAudio[i/drawAudioResolution].x*40,ofGetWidth()-PMDRAWSPACING-(i),ofGetHeight()-180+arrayForDrawAudio[i/drawAudioResolution].y*40);	
	}
	
}
