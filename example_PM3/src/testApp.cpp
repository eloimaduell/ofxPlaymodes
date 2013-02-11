#include "testApp.h"
using namespace ofxPm;
int resX,resY;
int sizeOfBuffer;
//--------------------------------------------------------------
void testApp::setup(){
	
	resX = 640;
	resY = 480;
	sizeOfBuffer = 3200;

	gui.setup();
	gui.add(fps.setup("fps",30,1,200));
	gui.add(delay.setup("delay",0,0,sizeOfBuffer*fps));
	gui.add(feedback.setup("feedback",0.0,0.0,1.0));
	grabber.initGrabber(resX,resY);	
	
	vFeed.setup(grabber, vHeader);
	vFeed.setInputOpacity(1.0);
	vFeed.setFeedback(feedback);
	vRate.setup(vFeed,fps);
	vBuffer.setup(vRate,sizeOfBuffer,true);
	vHeader.setup(vBuffer);
	
//	ofPixels blackPixels;
//	blackPixels.allocate(resX,resY,3);
//	blackPixels.set(0);
//	for(int i=0;i<400;i++){
//		VideoFrame videoFrame = VideoFrame::newVideoFrame(blackPixels);
//		//videoFrame.getTextureRef();
//		vBuffer.newVideoFrame(videoFrame);
//	}
	vHeader.setDelayMs(0);
	
	//vRenderer1.setup(grabber);
	vRenderer2.setup(vHeader);
	ofEnableAlphaBlending();
	ofBackground(0);
}

//--------------------------------------------------------------
void testApp::update(){
	grabber.update();
	vRate.setFps(fps);
	vHeader.setFps(fps);
	vHeader.setDelayMs(delay);
	vFeed.setFeedback(feedback);

}

//--------------------------------------------------------------
void testApp::draw(){
	//vBuffer.draw();
	//vHeader.draw();
	//vRenderer1.draw();
	ofPushMatrix();
	ofTranslate(640,0);
	vRenderer2.draw();
	ofPopMatrix();
	gui.draw();
	ofDrawBitmapString("FPS: " + ofToString(ofGetFrameRate())
					   + " || cameraBuffer FPS " + ofToString(float(vBuffer.getRealFPS())),20,ofGetHeight()-80);
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	
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
