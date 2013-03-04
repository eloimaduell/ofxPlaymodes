#include "testApp.h"
using namespace ofxPm;

//
// check this ofxFastFboReader !!
//

bool drawHelp = false;
bool runPdAudio = true;
int	soundStreamDeviceID = 5;
bool useVRate = true;

//--------------------------------------------------------------
void testApp::setup(){
	
	patchName = "basic.pd";
	durationInFrames = 60;
	grabFPS = 30.0;
	showGUI = false;
	fullscreen = true;
	pmVerbose = false;
	drawAudioResolution=1;	
	grabberResolution = ofVec2f(640,480);
	grabberAspectRatio = grabberResolution.x / grabberResolution.y;
	ofBackground(15,15,25);
	//ofSetVerticalSync(true);
	ofEnableAlphaBlending();

	setupGUI();
	
	/////////////////////////
	// ofxPlaymodes pipeline
	/////////////////////////
	
	/*
	[notice] device[8] Matrox HDTV 1080 - Matrox MXO2 - 1080i 50 YUV 8-bit
	[notice] device[9] Matrox HDTV 1080 - Matrox MXO2 - 1080i 50 YUV 10-bit
	[notice] device[10] Matrox HDTV 1080 - Matrox MXO2 - 1080i 59.94 YUV 8-bit
	[notice] device[11] Matrox HDTV 1080 - Matrox MXO2 - 1080i 59.94 YUV 10-bit
	[notice] device[12] Matrox HDTV 1080 - Matrox MXO2 - 1080PsF 23.98 YUV 8-bit
	[notice] device[13] Matrox HDTV 1080 - Matrox MXO2 - 1080PsF 23.98 YUV 10-bit
	[notice] device[14] Matrox HDTV 1080 - Matrox MXO2 - 1080p 23.98 YUV 8-bit
	[notice] device[15] Matrox HDTV 1080 - Matrox MXO2 - 1080p 23.98 YUV 10-bit
	[notice] device[16] Matrox HDTV 1080 - Matrox MXO2 - 1080p 25 YUV 8-bit
	[notice] device[17] Matrox HDTV 1080 - Matrox MXO2 - 1080p 25 YUV 10-bit
	[notice] device[18] Matrox HDTV 1080 - Matrox MXO2 - 1080p 29.97 YUV 8-bit
	[notice] device[19] Matrox HDTV 1080 - Matrox MXO2 - 1080p 29.97 YUV 10-bit
	[notice] device[20] Matrox HDTV 1080 - Matrox MXO2 - 1080p 24 YUV 8-bit
	[notice] device[21] Matrox HDTV 1080 - Matrox MXO2 - 1080p 24 YUV 10-bit
	[notice] device[22] Matrox HDTV 1080 - Matrox MXO2 - 1080p 30 YUV 8-bit
	[notice] device[23] Matrox HDTV 1080 - Matrox MXO2 - 1080p 30 YUV 10-bit
	[notice] device[24] Matrox HDTV 720 - Matrox MXO2 - 720p 50 YUV 8-bit
	[notice] device[25] Matrox HDTV 720 - Matrox MXO2 - 720p 50 YUV 10-bit
	[notice] device[26] Matrox HDTV 720 - Matrox MXO2 - 720p 59.94 YUV 8-bit
	[notice] device[27] Matrox HDTV 720 - Matrox MXO2 - 720p 59.94 YUV 10-bit
	[notice] device[28] Matrox HDTV 720 - Matrox MXO2 - 720p 23.98 YUV 8-bit
	[notice] device[29] Matrox HDTV 720 - Matrox MXO2 - 720p 23.98 YUV 10-bit
	[notice] device[30] Matrox HDTV 720 - Matrox MXO2 - 720p 25 YUV 8-bit
	[notice] device[31] Matrox HDTV 720 - Matrox MXO2 - 720p 25 YUV 10-bit
	[notice] device[32] Matrox HDTV 720 - Matrox MXO2 - 720p 29.97 YUV 8-bit
	[notice] device[33] Matrox HDTV 720 - Matrox MXO2 - 720p 29.97 YUV 10-bit
	[notice] device[34] Matrox NTSC - Matrox MXO2 - NTSC YUV 8-bit
	[notice] device[35] Matrox NTSC - Matrox MXO2 - NTSC YUV 10-bit
	[notice] device[36] Matrox PAL - Matrox MXO2 - PAL YUV 8-bit
	[notice] device[37] Matrox PAL - Matrox MXO2 - PAL YUV 10-bit
	*/
	
	// grabber
	//grabber.setDeviceID(8);
	//grabber.listDevices();
	grabber.setDesiredFrameRate(grabFPS);
	grabber.setFps(grabFPS);	
	grabber.initGrabber(grabberResolution.x,grabberResolution.y);
	
	// feedback	
	vFeedback.setup(grabber,vHeader);
	vFeedback.setFeedback(feedBack);
	vFeedback.setInputOpacity(opacityIn);

	// vRate
	if(useVRate) vRate.setup(vFeedback,recRate);

	// vBuffer
	if (useVRate) vBuffer.setup(vRate,durationInFrames, true);
	else vBuffer.setup(vFeedback,durationInFrames, true);

	// vHeader
	vHeader.setup(vBuffer);
	vHeader.setDelayMs(delay);
	vHeader.setInMs(in);
	vHeader.setOutMs(out);
	vHeader.setLengthMs((float(durationInFrames)/float(grabFPS))*1000.0);	
	vHeader.setWindowPriority("length");
	
	// vDryWet
	vDryWet.setup(vFeedback ,vHeader);
	vDryWet.setDryWet(dryWet);
	
	// renderers
	vRendererGrabber.setup(grabber);
	vRendererBuffer.setup(vBuffer);
	vRendererHeader.setup(vHeader);
	vRendererFinal.setup(vDryWet);

	
	////////////////////
	// pd stuff
	////////////////////
	if(runPdAudio)
	{
		// this index is the pd_index of the array in pd which works as circular buffer and it indicates the recording header
		indexArrayFromPd=0;	
		sampleRate = 96000;
		// puredata works on sounds in chunks of 64 samples (called a tick)
		// 8 ticks per buffer corresponds to 8 * 64 sample buffer size (512 samples)
		int ticksPerBuffer = 8;
		
		// this initialized pd
		// format is pd.init(numOutputChannels, numInputChannels, samplerate, ticksPerBuffer)
		// note that outputs comes before inputs.
		pd.init(2, 2, sampleRate, ticksPerBuffer);
		
		// load external libs
		expr_setup();
		zexy_setup();
		susloop_tilde_setup();
		lrshift_tilde_setup();
		
		// open the patch (relative to the data folder)	
		pd.openPatch(patchName);
		
		// start pd
		pd.computeAudio(true);
		
		pd.subscribe("delayDoppler");
		pd.subscribe("delayPlay");
		pd.subscribe("levelEnvGrain");
		pd.subscribe("levelEnvFollowIn");
		pd.subscribe("opacityEnv");
		pd.subscribe("index");
		pd.addReceiver(*this);
			
		// start the sound stream - always do this last in your setup
		
		//soundStream.listDevices();
		//soundStream.setDeviceID(soundStreamDeviceID);
		//soundStream.setup(2,2, sampleRate, 64*ticksPerBuffer,1);
		ofSoundStreamSetup(2, 2, this, sampleRate, 64*ticksPerBuffer, 2);

		// init pd object
		pd.sendFloat("sampleRate",float(sampleRate));
		pd.sendFloat("frameRate",grabFPS);
		pd.sendFloat("bufferSize",sampleRate * (float(durationInFrames) / grabFPS));
		pd.sendFloat("speed",1.0f);
		pd.sendFloat("grainKeepDuration",0.0f);
		pd.sendFloat("doppler",0.0f);
		pd.sendFloat("delay",0.0f);
		pd.sendFloat("feedback",0.0f);
		pd.sendFloat("opacityOut",1.0f);
		pd.sendFloat("in",(float(durationInFrames)/float(grabFPS))*1000.0);
		pd.sendFloat("fullGrain",0.0f);
		pd.sendFloat("grainAttack",0.0f);
		pd.sendFloat("grainDecay",0.0f);
		pd.sendFloat("loopType",3.0f);
		pd.sendFloat("dryWet",1.0f);
		pd.sendFloat("controlTimeStretch",0.0f);
		pd.sendFloat("opacityEnvToggle",0.0f);
		pd.sendFloat("levelEnvMultiplyer",1.0f);
		pd.sendFloat("levelPlayAudioFrame",0.0f);
		pd.sendFloat("controlPitchStretch",0.0f);

		//pd.sendFloat("out",0.0f);
		pd.sendFloat("length",(float(durationInFrames)/float(grabFPS))*1000.0);
		pd.sendFloat("play",0.0f);
		pd.sendFloat("rec",1.0f);

		printf("//////////////// pd object \n\n");
		printf("pd object : samplerate %f || frameRate %f\n",sampleRate, grabFPS);
		printf("pd object : bufferSize %f\n",sampleRate * (float(durationInFrames) / grabFPS));
		printf("pd object : gran in and length %f\n",(float(durationInFrames)/float(grabFPS))*1000.0);
		printf("//////////////// pd object \n\n");
	}
		
	
	////////////////////
	// OSC stuff
	////////////////////
	oscReceiver.setup(12345,"/pm3");
	ofAddListener(oscReceiver.newOscMessage,this,&testApp::newOscMessageReceived);

}


//--------------------------------------------------------------
void testApp::update()
{

	// ready array from Pd
	if((drawAudioToggle || rec) && runPdAudio) pd.readArray("playmodes", arrayFromPd);
	
	if(videoFileGrabber.isLoaded()) {}//videoFileGrabber.update();
	else grabber.update();


}

//--------------------------------------------------------------
void testApp::draw(){
	
	if(!fullscreen)
	{
		ofSetColor(255);
		ofPushMatrix();
		ofTranslate(300,20);
		float resizeFactorX = (ofGetWidth()-320.0f-20.0f)/3.0f;
		float resizeFactorY = ((ofGetWidth()-320.0f-20.0f)/grabberAspectRatio)/3.0f;
		vRendererGrabber.draw(0,0,resizeFactorX,resizeFactorY);
		vRendererBuffer.draw(0,20+resizeFactorY,resizeFactorX,resizeFactorY);
		vRendererHeader.draw(0,20+20+resizeFactorY*2,resizeFactorX,resizeFactorY);
		ofPopMatrix();

		ofPushMatrix();
		ofTranslate(320.0+resizeFactorX,20.0f);
		ofSetColor(vHeader.getOpacity());
		vRendererFinal.draw(0,0,resizeFactorX*2.0f,resizeFactorY*2.0f);
		ofPopMatrix();
			
	}
	else 
	{
		ofBackground(0);
		ofSetColor(255);
		float screenAspectRatio = float(ofGetWidth()) / float(ofGetHeight());
		ofSetColor(vHeader.getOpacity());
		if (screenAspectRatio>grabberAspectRatio) 
		{
			vRendererFinal.draw((ofGetWidth()-ofGetHeight()*grabberAspectRatio)/2,0,ofGetHeight()*grabberAspectRatio,ofGetHeight());
		}
		else 
		{
			vRendererFinal.draw(0,0,ofGetWidth(),ofGetWidth()/grabberAspectRatio);
		}
	}
	if(drawBufferToggle)
	{		
		vBuffer.draw();
		vHeader.draw();
	}
	
	if(drawAudioToggle && runPdAudio) drawPdBuffer();

	if(showGUI)
	{
		controlPanel.draw();
		configPanel.draw();
		levelsPanel.draw();
		grainPanel.draw();
		envFollowPanel.draw();
		
		if(pd.isInited())
			ofSetColor(128);
		else 
			ofSetColor(128,0,0);
		
		ofDrawBitmapString(vHeader.getInfo(),20,ofGetHeight()-20);
	}

	// draw FPS indicator
	ofSetColor(0,255,0);
	ofDrawBitmapString(patchName + "  ||  " + ofToString(int(ofGetFrameRate()))
					   + " fps "
					   + " || buffer real fps " + ofToString(vBuffer.getRealFPS()) + " fps"
					   ,20,ofGetHeight()-35);
	
}
//--------------------------------------------------------------
void testApp::keyPressed(int key){

	printf("key %d\n",key);
	string presetNum = "";
	int loadOrSave = -1;
	
	if(key=='s') grabber.videoSettings();
	else if(key=='f') 
	{
		fullscreen=!fullscreen;
		ofToggleFullscreen();
		if(!fullscreen) showGUI=true;
		else showGUI=false;
	}
	else if(key=='F') 
	{
		fullscreen=!fullscreen;
		//ofToggleFullscreen();
		if(!fullscreen) showGUI=true;
		else showGUI=false;
	}
	else if(key=='h') 
	{
		showGUI=!showGUI;
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
		loadOrSave = 1;
		presetNum = "1"; 
	}
	else if (key=='!')
	{
		loadOrSave = 2;
		presetNum = "1"; 
	}
	else if (key=='2')
	{
		loadOrSave = 1;
		presetNum = "2"; 
	}
	else if (key=='"')
	{
		loadOrSave = 2;
		presetNum = "2"; 
	}
	else if (key=='3')
	{
		loadOrSave = 1;
		presetNum = "3"; 
	}
	else if (key==183)
	{
		loadOrSave = 2;
		presetNum = "3"; 
	}
	else if (key=='4')
	{
		loadOrSave = 1;
		presetNum = "4"; 
	}
	else if (key=='$')
	{
		loadOrSave = 2;
		presetNum = "4"; 
	}
	else if (key=='5')
	{
		loadOrSave = 1;
		presetNum = "5"; 
	}
	else if (key=='%')
	{
		loadOrSave = 2;
		presetNum = "5"; 
	}
	else if (key=='6')
	{
		loadOrSave = 1;
		presetNum = "6"; 
	}
	else if (key=='&')
	{
		loadOrSave = 2;
		presetNum = "6"; 
	}
	else if (key=='7')
	{
		loadOrSave = 1;
		presetNum = "7"; 
	}
	else if (key=='/')
	{
		loadOrSave = 2;
		presetNum = "7"; 
	}
	else if (key=='8')
	{
		loadOrSave = 1;
		presetNum = "8"; 
	}
	else if (key=='(')
	{
		loadOrSave = 2;
		presetNum = "8"; 
	}
	else if (key=='9')
	{
		loadOrSave = 1;
		presetNum = "9"; 
	}
	else if (key==')')
	{
		loadOrSave = 2;
		presetNum = "9"; 
	}
	else if (key=='0')
	{
		loadOrSave = 1;
		presetNum = "0"; 
	}
	else if (key=='=')
	{
		loadOrSave = 2;
		presetNum = "0"; 
	}
	else if (key=='-')
	{
		pmVerbose=!pmVerbose;
	}
	
	
	// load or save preset 
	if (loadOrSave==1)
	{
		configPanel.loadFromFile("./presets/configPanel_" +presetNum +".xml");
		levelsPanel.loadFromFile("./presets/levelsPanel_"+presetNum +".xml");
		grainPanel.loadFromFile("./presets/grainPanel_"+presetNum +".xml");
		controlPanel.loadFromFile("./presets/controlPanel_"+presetNum +".xml");
		envFollowPanel.loadFromFile("./presets/envFollowPanel_"+presetNum +".xml");
		presetLabel = ofToString("preset : "+ presetNum);
		
	}
	else if (loadOrSave==2)
	{
		configPanel.saveToFile("./presets/configPanel_"+presetNum +".xml");
		levelsPanel.saveToFile("./presets/levelsPanel_"+presetNum +".xml");
		grainPanel.saveToFile("./presets/grainPanel_"+presetNum +".xml");
		envFollowPanel.saveToFile("./presets/envFollowPanel_"+presetNum +".xml");
		controlPanel.saveToFile("./presets/controlPanel_" + presetNum +".xml");		
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
	
	/*

	 grabber.setDesiredFrameRate(30);
	 grabber.setFps(30);	
	 grabber.initGrabber(grabberResolution.x,grabberResolution.y);
	 
	 //vRate.setup(grabber,fps);
	 
	 vBuffer.setup(vRate,durationInFrames, false);
	 
	 vHeader.setup(vBuffer);
	 vHeader.setDelayMs(delay);
	 vHeader.setInMs(in);
	 vHeader.setOutMs(out);
	 vHeader.setLengthMs(13333.0);	
	 vHeader.setWindowPriority("length");
	 
	 vFeedback.setup(grabber,vHeader);
	 vFeedback.setFeedback(feedBack);
	 vFeedback.setInputOpacity(opacityIn);
	 vRate.setup(vFeedback,fps);
	 
	 vDryWet.setup(vFeedback ,vHeader);
	 vDryWet.setDryWet(dryWet);
	 
	 vRendererBuffer.setup(grabber);
	 vRendererHeader.setup(vDryWet);
	 
	 
	 */	
	
	
	/* 
	 
	 NEW BUT BAD
	 
	videoFileGrabber.loadMovie(dragInfo.files[0]);
	durationInFrames = videoFileGrabber.getTotalNumFrames();
	grabberResolution = ofVec2f(videoFileGrabber.getWidth(),videoFileGrabber.getHeight());
	grabberAspectRatio = grabberResolution.x / grabberResolution.y;

	printf("videoFileGrabber:: dragged movie %s . DurationInFrames %d \n",dragInfo.files[0].c_str(),durationInFrames);

	
	vHeader.setup(vBuffer);
	vHeader.setDelayMs(0.0f);
	vHeader.setInMs(videoFileGrabber.getDuration()*1000.0);
	vHeader.setOutMs(out);
	vHeader.setLengthMs(videoFileGrabber.getDuration()*1000.0);	
	vHeader.setWindowPriority("length");
	
	vBuffer.stop();
	vRate.removeListener();

	vBuffer.setup(videoFileGrabber,durationInFrames);
	vBuffer.resume();
	
	vFeedback.setup(grabber,vHeader);
//	vFeedback.setFeedback(feedBack);
//	vFeedback.setInputOpacity(opacityIn);
//	vRate.setup(vFeedback,fps);
	
//	vDryWet.setup(vFeedback ,vHeader);
//	vDryWet.setDryWet(dryWet);
	
	vRendererBuffer.setup(grabber);
	vRendererHeader.setup(vDryWet);
	
	for(int i=0;i<durationInFrames;i++){
		printf("-%d",i);
		videoFileGrabber.setFrame(i);
		VideoFrame videoFrame = VideoFrame::newVideoFrame(videoFileGrabber.getPixelsRef());
		videoFrame.getTextureRef();
		vBuffer.newVideoFrame(videoFrame);
	}	
	
	*/
	
	
///* old but good  2 *****************
 
/*
	videoFileGrabber.loadMovie(dragInfo.files[0]);
	durationInFrames = videoFileGrabber.getTotalNumFrames();
	grabberResolution = ofVec2f(VideoSource(videoFileGrabber).getWidth(),VideoSource::videoFileGrabber.getHeight());
	grabberAspectRatio = grabberResolution.x / grabberResolution.y;
*/			
	//?vRate.removeListener();
	vFeedback.removeListener();
	vDryWet.removeListener();
	
	vBuffer.stop();
	rec=false;
	vBuffer.setup(videoFileGrabber,durationInFrames);

	//vBuffer.setInSource(vHeader);
	vHeader.setup(vBuffer);
	vHeader.setDelayMs(0.0f);
	vHeader.setLengthMs(videoFileGrabber.getDuration()*1000.0);
	vHeader.setInMs(videoFileGrabber.getDuration()*1000.0);
	//	vHeader.setOutMs(out);
	vHeader.setWindowPriority("length");

	//vDryWet.setup(vHeader,videoFileGrabber);
	//vDryWet.setDryWet(dryWet);

	vRendererBuffer.setup(videoFileGrabber);
	vRendererFinal.setup(vHeader);
		
	for(int i=0;i<200;i++)
	{
		videoFileGrabber.setFrame(i);
		VideoFrame videoFrame = VideoFrame::newVideoFrame(videoFileGrabber.getPixelsRef());
		videoFrame.getTextureRef();
		vBuffer.newVideoFrame(videoFrame);
	}	
	

//****************** */
	
	
	/*
	//old but good *****************
	
	//	fboA.allocate(grabberResolution.x, grabberResolution.y,GL_RGB,0);
	//	fboFeedback.allocate(grabberResolution.x, grabberResolution.y,GL_RGB,0);
	//	pixelsToBuffer.allocate(grabberResolution.x, grabberResolution.y,OF_PIXELS_RGB);
	
	videoFileGrabber.loadMovie(dragInfo.files[0]);
	durationInFrames = videoFileGrabber.getTotalNumFrames();
	grabberResolution = ofVec2f(videoFileGrabber.getWidth(),videoFileGrabber.getHeight());
	grabberAspectRatio = grabberResolution.x / grabberResolution.y;
	
	
	vHeader.setDelayMs(0.0f);
	vHeader.setLengthMs(videoFileGrabber.getDuration()*1000.0);
	vHeader.setInMs(videoFileGrabber.getDuration()*1000.0);
	//	vHeader.setOutMs(out);
	vHeader.setWindowPriority("length");
	vBuffer.stop();
	vRate.removeListener();
	
	vBuffer.setup(videoFileGrabber,durationInFrames);
	vBuffer.resume();
	//vBuffer.setInSource(vHeader);
	vHeader.setup(vBuffer);
	vRendererBuffer.setup(videoFileGrabber);
	vRendererHeader.setup(vHeader);
	
	//videoFileGrabber.play();
	
	for(int i=0;i<200;i++)
	{
		videoFileGrabber.setFrame(i);
		VideoFrame videoFrame = VideoFrame::newVideoFrame(videoFileGrabber.getPixelsRef());
		videoFrame.getTextureRef();
		vBuffer.newVideoFrame(videoFrame);
	}	
	
	
	
	//vBuffer.setPixelsIn(pixelsToBuffer);
	
	//****************** */
	
	
	
	
	printf("dragEvent : file %s [%d frames] [duration %f]\n",dragInfo.files[0].c_str(),durationInFrames,videoFileGrabber.getDuration()*1000.0);
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
	if(pmVerbose)printf(">receive float from Pd %s %f\n",dest.c_str(),value);
	float dryWetAlpha = 0.09;
	if(dest=="delayDoppler") 
	{
		//vHeader.setDelayMs(value/(sampleRate/1000.0f));
		//if(pmVerbose)printf("received from pd> delayDoppler!!\n");		
	}
	else if(dest=="levelEnvGrain") 
	{
//		if(opacityEnvToggle) opacityIn=value*10.0;
		//if(opacityEnvToggle) dryWet=((1.0f-dryWetAlpha)*dryWet)+(dryWetAlpha*value*10.0);
		//if(dryWet>1.0) dryWet=1.0;
		opacityOut=value;
		if(pmVerbose)printf("received from pd> levelEnvGrain!!\n");		
	}
	else if(dest=="levelEnvFollowIn")
	{
		
		if(pmVerbose)printf("received from pd> levelEnvFollowIn :: %f!!\n",value);		
		opacityIn=value;
		//opacityInChanged(value);
	}
	else if(dest=="index")
	{
		/*if(!play)*/ indexArrayFromPd = int(value);
	}
	else if(dest=="delayPlay")
	{
		//vHeader.setDelayMs(value/(sampleRate/1000.0f));
		vHeader.setDelayMs(value);
		if(pmVerbose)printf("received from pd > delayPlay %f\n",value/(sampleRate/1000.0f));
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
	pd.sendFloat("in",f);
	if(pmVerbose)printf("> in changed to %f\n",f);
}
//--------------------------------------------------------------
void testApp::outChanged(float &f)
{
	vHeader.setOutMs(f);
	pd.sendFloat("out",f);
	if(pmVerbose)printf("> out changed to %f\n",f);

}
//--------------------------------------------------------------
void testApp::lengthChanged(float &f)
{
	vHeader.setLengthMs(f);
	pd.sendFloat("length",f);
	if(pmVerbose)printf("> length changed to %f\n",f);
}
//--------------------------------------------------------------
void testApp::delayChanged(float &f)
{
	if(pmVerbose)printf("> delay changed to %f\n",f);
	//vHeader.setDelayMs(f);
	pd.sendFloat("delay",f);
	
}
//--------------------------------------------------------------
void testApp::speedChanged(float &f)
{
	vHeader.setSpeed(f);
	pd.sendFloat("speed",f);
 	if(pmVerbose)printf("> speed changed to %f\n",f);
}
//--------------------------------------------------------------
void testApp::feedBackChanged(float &f)
{
	pd.sendFloat("feedBack",f);
	//vBuffer.setFeedBack(f);
	vFeedback.setFeedback(f);
	if(pmVerbose)printf("> feedback changed to %f\n",f);
}
//--------------------------------------------------------------
void testApp::opacityOutChanged(float &f)
{
	vHeader.setOpacity(int(f*255.0));
	pd.sendFloat("opacityOut",f);
	if(pmVerbose) printf("> opacity out changed to %f\n",f);
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
	if(pmVerbose)printf("> opacity in changed to %f\n",f);
//	}
	vFeedback.setInputOpacity(f);

}
//--------------------------------------------------------------
void testApp::attackGrainChanged(float &f)
{
	pd.sendFloat("grainAttack",f);
	vHeader.setAttack(f);

	if(pmVerbose)printf(">attack changed to %f\n",f);
}
//--------------------------------------------------------------
void testApp::decayGrainChanged(float &f)
{
	pd.sendFloat("grainDecay",f);
	vHeader.setDecay(f);
	
	if(pmVerbose)printf(">decay changed to %f\n",f);
}
//--------------------------------------------------------------
void testApp::dryWetChanged(float &f)
{
	pd.sendFloat("dryWet",f);
	vDryWet.setDryWet(dryWet);
	if(pmVerbose)printf(">dryWet changed to %f\n",f);
}

//--------------------------------------------------------------
void testApp::levelEnvMultiplyerChanged(float &f)
{
	pd.sendFloat("levelEnvMultiplyer",f);
	if(pmVerbose)printf(">levelEnvMultiplyer changed to %f\n",f);
}


//--------------------------------------------------------------
void testApp::controlPitchStretchChanged(float &f)
{
	pd.sendFloat("controlPitchStretch",f);
	if(pmVerbose)printf(">controlPitchStretch changed to %f\n",f);
}


//--------------------------------------------------------------
// int change's
//--------------------------------------------------------------
void testApp::recRateChanged(int &i)
{
	//? vRate.setFps(i);
	if(pmVerbose)printf(">recRate changed to %d\n",i);

}
//--------------------------------------------------------------
void testApp::avSyncOffsetChanged(int &i)
{
	vHeader.setOffsetInFrames(i);
	if(pmVerbose)printf(">avSync offset changed to %d\n",i);
}
//--------------------------------------------------------------
void testApp::scratchFactorChanged(int &i)
{
	pd.sendFloat("doppler",i);
	if(pmVerbose)printf(">scratch factor changed to %d\n",i);
}
//--------------------------------------------------------------
void testApp::loopTypeChanged(int &i)
{
	pd.sendFloat("loopType",i);
	vHeader.setLoopMode(i);
	if(pmVerbose)printf(">loopTypeChanged to %d\n",i);
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
		if(pmVerbose)printf("> rec stop 0.0\n");
	}
	else
	{
		vBuffer.resume();
		pd.sendFloat("rec",1.0);
		if(pmVerbose)printf("> rec play changed to 1.0\n");
	}
	
	
}
//--------------------------------------------------------------
void testApp::playChanged(bool &b)
{
	if(b) {
		vHeader.setPlaying(true);
		vHeader.setDriveMode(1);
		pd.sendFloat("play",1.0);
		if(pmVerbose)printf("> play changed to 1.0\n");
		
	}
	else 
	{
		vHeader.setPlaying(false);
		vHeader.setDelayMs(delay);
		vHeader.setDriveMode(0);

		pd.sendFloat("play",0.0);
		if(pmVerbose)printf("> play changed to 0.0\n");
	}	
}
//--------------------------------------------------------------
void testApp::drawAudioChanged(bool &b)
{
}
//--------------------------------------------------------------
void testApp::verboseToggleChanged(bool &b)
{
	pmVerbose=b;
}


//--------------------------------------------------------------
void testApp::keepDurationToggleChanged(bool &b)
{
	if(b) pd.sendFloat("grainKeepDuration",1.0f);
	else pd.sendFloat("grainKeepDuration",0.0f);
}

//--------------------------------------------------------------
void testApp::fullGrainChanged(bool &b)
{
	if(b)
	{
		pd.sendFloat("fullGrain",1.0f);
		if(pmVerbose)printf("> full grain changed to 1.0\n");
	}
	else 
	{
		pd.sendFloat("fullGrain",0.0f);
		if(pmVerbose)printf("> full grain changed to 0.0\n");
	}
}
//-------------------------------------------------------------
void testApp::drawBufferChanged(bool &b)
{

}
//-------------------------------------------------------------
void testApp::opacityEnvToggleChanged(bool &b)
{
	
	if(b)
	{
		pd.sendFloat("opacityEnvToggle",1.0f);
		if(pmVerbose)printf("> oppacity Env Toggle changed to 1.0\n");
	}
	else 
	{
		pd.sendFloat("opacityEnvToggle",0.0f);
		if(pmVerbose)printf("> oppacity Env Toggle changed to 0.0\n");
	}
	
}

//-------------------------------------------------------------
void testApp::grabberSettingsToggleChanged(bool &b)
{
	if(b)
	{
		grabber.videoSettings();
		b = grabberSettingsToggle =false;
		if(pmVerbose)printf("> grabber video settings \n");
	}
	

}

//-------------------------------------------------------------
void testApp::playAudioFrameToggleChanged(bool &b)
{
	if(pmVerbose)printf("> play audio frame changed \n");
	
	if (b) pd.sendFloat("levelPlayAudioFrame",1.0f);
	else pd.sendFloat("levelPlayAudioFrame",0.0f);
	
}
//-------------------------------------------------------------
void testApp::timeStretchToggleChanged(bool &b)
{
	if(pmVerbose)printf("> time stretch changed \n");
	
	if (b) pd.sendFloat("controlTimeStretch",1.0f);
	else pd.sendFloat("controlTimeStretch",0.0f);
	
}


//-------------------------------------------------------------
void testApp::setupGUI()
{

	////////////////////
	// ofxGUI
	////////////////////
	configPanel.setup("CONFIG",			"./defaultSettings/configSettings.xml", 10,10);
	controlPanel.setup("CONTROL",		"./defaultSettings/controlSettings.xml",10,160);
	levelsPanel.setup("LEVELS",			"./defaultSettings/levelsSettings.xml", 10, 350);
	envFollowPanel.setup("ENV_FOLLOW",	"./defaultSettings/envFollowSettings.xml", 10, 490);	
	grainPanel.setup("GRAIN",			"./defaultSettings/grainSettings.xml", 10, 555);

	controlPanel.loadFromFile("./defaultSettings/controlSettings.xml");
	configPanel.loadFromFile("./defaultSettings/configSettings.xml");
	levelsPanel.loadFromFile("./defaultSettings/levelsSettings.xml");
	grainPanel.loadFromFile("./defaultSettings/grainSettings.xml");
	envFollowPanel.loadFromFile("./defaultSettings/envFollowSettings.xml");

	configPanel.add(recRate.setup				("rec at fps",int(grabFPS),15,60));
	configPanel.add(avSyncOffset.setup			("av-sync offset",0,-50,50));
	configPanel.add(drawAudioToggle.setup		("draw audio",true));	
	configPanel.add(drawBufferToggle.setup		("draw buffer",true));
	configPanel.add(grabberSettingsToggle.setup	("grabber settings",false));		
	configPanel.add(verbose.setup				("print console",false));		
	controlPanel.add(presetLabel.setup			(ofToString("presetLabel"),patchName + ofToString(" :: none")));
	controlPanel.add(rec.setup					("rec",true));
	controlPanel.add(play.setup					("play",false));
	controlPanel.add(speed.setup				("speed",1.0,-4.0,4.0));
	controlPanel.add(delay.setup				("delay ms",0.0,0.0,(float(durationInFrames)/float(recRate))*1000.0));
	controlPanel.add(scratchFactor.setup		("scratch factor",0.0,0.0,4000.0));
	controlPanel.add(timeStretchToggle.setup	("time stretch",false));
	controlPanel.add(controlPitchStretch.setup	("pitch on stretch",1.0,-36.0,36.0));
	
	envFollowPanel.add(opacityEnvToggle.setup	("opacity env",false));
	envFollowPanel.add(levelEnvMultiplyer.setup	("mic sensitivity",1.0,0.0,20.0));

	levelsPanel.add(opacityIn.setup				("level in",1.0,0.0,1.0));
	levelsPanel.add(opacityOut.setup			("level out",1.0,0.0,1.0));
	levelsPanel.add(feedBack.setup				("feedBack",0.0,0.0,1.0));
	levelsPanel.add(dryWet.setup				("dry / wet",1.0,0.0,1.0));
	levelsPanel.add(playAudioFrameToggle.setup	("play audio frame",false));	
	
	grainPanel.add(length.setup					("length ms",(float(durationInFrames)/float(recRate))*1000.0,0.0,(float(durationInFrames)/float(recRate))*1000.0));
	grainPanel.add(in.setup						("in ms",(float(durationInFrames)/float(recRate))*1000.0,0.0,(float(durationInFrames)/float(recRate))*1000.0));
	grainPanel.add(out.setup					("out ms",0.0,0.0,(float(durationInFrames)/float(recRate))*1000.0));
	grainPanel.add(attackGrain.setup			("attack grain",0.0,0.0,2000.0));
	grainPanel.add(decayGrain.setup				("decay grain",0.0,0.0,2000.0));
	grainPanel.add(fullGrain.setup				("play full grain",false));
	grainPanel.add(keepDurationToggle.setup		("keep duration",false));
	grainPanel.add(loopType.setup				("loopType",3,1,3));
	
	////////////////////
	// - add listener's for events.	
	////////////////////
	controlPanel.getFloatSlider("delay ms").addListener(this,&testApp::delayChanged);
	controlPanel.getFloatSlider("speed").addListener(this,&testApp::speedChanged);
	controlPanel.getIntSlider("scratch factor").addListener(this,&testApp::scratchFactorChanged);	
	controlPanel.getToggle("rec").addListener(this,&testApp::recChanged);	
	controlPanel.getToggle("play").addListener(this,&testApp::playChanged);	
	controlPanel.getToggle("time stretch").addListener(this,&testApp::timeStretchToggleChanged);	
	controlPanel.getFloatSlider("pitch on stretch").addListener(this,&testApp::controlPitchStretchChanged);	
	

	envFollowPanel.getToggle("opacity env").addListener(this,&testApp::opacityEnvToggleChanged);	
	envFollowPanel.getFloatSlider("mic sensitivity").addListener(this,&testApp::levelEnvMultiplyerChanged);
	
	grainPanel.getFloatSlider("in ms").addListener(this,&testApp::inChanged);
	grainPanel.getFloatSlider("out ms").addListener(this,&testApp::outChanged);
	grainPanel.getFloatSlider("length ms").addListener(this,&testApp::lengthChanged);
	grainPanel.getFloatSlider("attack grain").addListener(this,&testApp::attackGrainChanged);
	grainPanel.getFloatSlider("decay grain").addListener(this,&testApp::decayGrainChanged);
	grainPanel.getIntSlider("loopType").addListener(this,&testApp::loopTypeChanged);	
	grainPanel.getToggle("play full grain").addListener(this,&testApp::fullGrainChanged);	
	grainPanel.getToggle("keep duration").addListener(this,&testApp::keepDurationToggleChanged);	
	
	levelsPanel.getFloatSlider("feedBack").addListener(this,&testApp::feedBackChanged);
	levelsPanel.getFloatSlider("level in").addListener(this,&testApp::opacityInChanged);
	levelsPanel.getFloatSlider("level out").addListener(this,&testApp::opacityOutChanged);
	levelsPanel.getFloatSlider("dry / wet").addListener(this,&testApp::dryWetChanged);
	levelsPanel.getToggle("play audio frame").addListener(this,&testApp::playAudioFrameToggleChanged);
	
	configPanel.getIntSlider("rec at fps").addListener(this,&testApp::recRateChanged);
	configPanel.getIntSlider("av-sync offset").addListener(this,&testApp::avSyncOffsetChanged);
	configPanel.getToggle("draw audio").addListener(this,&testApp::drawAudioChanged);	
	configPanel.getToggle("draw buffer").addListener(this,&testApp::drawBufferChanged);	
	configPanel.getToggle("grabber settings").addListener(this,&testApp::grabberSettingsToggleChanged);	
	configPanel.getToggle("print console").addListener(this,&testApp::verboseToggleChanged);	
	
	
}

//--------------------------------------------------------------
void testApp::newOscMessageReceived(pmOscMessage &mess)
{
	if(pmVerbose)printf("testApp:: OSC in  :: %s : %f \n",mess.address.c_str(),mess.value);
	
	// FLOAT 
	if(mess.address=="/pm3/header/delay")
	{
		delayChanged(mess.value);
	}
	else if(mess.address=="/pm3/header/in")
	{
		inChanged(mess.value);
	}
	else if(mess.address=="/pm3/header/length")
	{
		lengthChanged(mess.value);
	}
	else if(mess.address=="/pm3/header/speed")
	{
		speedChanged(mess.value);
	}
	else if(mess.address=="/pm3/master/out")
	{
		opacityOutChanged(mess.value);
	}
	else if(mess.address=="/pm3/master/in")
	{
		opacityInChanged(mess.value);
	}
	else if(mess.address=="/pm3/buffer/feedback")
	{
		feedBackChanged(mess.value);
	}
	else if(mess.address=="/pm3/buffer/scratchDecayTime")
	{
		int i = int(mess.value);
		scratchFactorChanged(i);
	}
	
	// BOOLEANS 
	else if(mess.address=="/pm3/header/play")
	{
		bool b;
		if(mess.value==1.0){
			b=true;
		}
		else {
			b=false;
		}
		playChanged(b);
	}
	else if(mess.address=="/pm3/header/rec")
	{
		bool b;
		if(mess.value==1.0){
			b=true;
		}
		else {
			b=false;
		}
		recChanged(b);
	}
	else if(mess.address=="/pm3/header/enableTimeStretch")
	{
		bool b;
		if(mess.value==1.0){
			b=true;
		}
		else {
			b=false;
		}
		timeStretchToggleChanged(b);
	}
	else if(mess.address=="/pm3/header/keepWholeGrain")
	{
		bool b;
		if(mess.value==1.0){
			b=true;
		}
		else {
			b=false;
		}
		fullGrainChanged(b);
	}
	
}
