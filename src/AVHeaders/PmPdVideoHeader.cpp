/*
 * AudioFrame.h
 *
 *  Created on: 09-oct-2008
 *      Author: arturo castro
 */

#include "PmPdVideoHeader.h"

namespace ofxPm{
//PmPdVideoHeader::PmPdVideoHeader(VideoBuffer & buffer){
//setup(buffer);
//}

//------------------------------------------------------
PmPdVideoHeader::PmPdVideoHeader(){
	attack = 0.0;
	decay = 0.0;
}

//------------------------------------------------------
PmPdVideoHeader::~PmPdVideoHeader(){
}

//------------------------------------------------------
void PmPdVideoHeader::draw(){
	
	ofEnableAlphaBlending();
	
	
    double oneLength=(double)(ofGetWidth()-PMDRAWSPACING*2)/(double)(buffer->getMaxSize());
    double currentLength=double(currentPos)/(double(this->buffer->getMaxSize()))*(double)(ofGetWidth()-PMDRAWSPACING*2);
	int headerPos = (buffer->size()-currentPos+offsetFrames) * oneLength;
	int drawHeaderY = ofGetHeight() -140;
	int originXAtEnd = ofGetWidth() - PMDRAWSPACING;
	int drawBufferY = ofGetHeight() -80;
	
	// draw header line
    ofSetLineWidth(1);
	ofLine(originXAtEnd-headerPos+1,drawHeaderY+50,originXAtEnd-headerPos,drawHeaderY-40);
	if(playing) ofSetColor(0,200,0);
	else ofSetColor(255,128,0);
	ofRect(originXAtEnd-headerPos-offsetFrames,drawHeaderY+50,oneLength,10);
	ofDrawBitmapString(ofToString(buffer->size()-currentPos)+"/"+ofToString(currentPos+1),ofPoint(originXAtEnd-headerPos-offsetFrames-(oneLength/2),drawHeaderY+45));
	
	
	//int	inFrame  = int(double(buffer->size()-1)*(in));
	//int outFrame = int(double(buffer->size()-1)*(out));
	int inFrame = this->getInFrames();
	int outFrame = this->getOutFrames();
	int inPos;
	//inPos= PMDRAWSPACING + ((int(buffer->size())-inFrame) * oneLength) + oneLength/2;
	inPos = originXAtEnd - (oneLength*(inFrame+1));
	//int outPos = PMDRAWSPACING + (int((buffer->size())-outFrame) * oneLength) + oneLength/2;
	int outPos = originXAtEnd - (oneLength*outFrame);
	
	//printf("inPos %d outPos %d :: in %f inF %d oneL %f buffS %d:: %d\n",inPos,outPos,in,inFrame,oneLength, ((buffer->size()-1-inFrame) * oneLength),PMDRAWSPACING + ((buffer->size()-1-inFrame) * oneLength) + (oneLength/2));
	

	// draw in & out lines
	ofSetLineWidth(1.0);
	ofSetColor(255,128,0);
	ofLine(inPos,drawHeaderY-40,inPos,drawHeaderY+60);
    ofLine(outPos,drawHeaderY-40,outPos,drawHeaderY+60);
	float fps=30.0;
	// draw in & out rectangle with attack decay
	int inAttackPos = inPos + int((attack*fps/1000.0)*oneLength);
	int outDecayPos = outPos - int((decay*fps/1000.0)*oneLength);
	ofSetColor(255,128,0,40);
	ofFill();
	ofBeginShape();
	ofVertex(inPos,drawBufferY);
	ofVertex(outPos,drawBufferY);
	ofVertex(outDecayPos,drawHeaderY-40);	
	ofVertex(inAttackPos,drawHeaderY-40);
	ofEndShape(false);
	
	//ofRect(inPos,drawHeaderY,outPos-inPos,50);
	
	//ofLine(inPos,drawHeaderY+60,outPos,drawHeaderY+60);
	// draw inPos triangle
	ofSetColor(255,128,0);
	ofBeginShape();
		ofVertex(inPos,drawHeaderY+10);
		ofVertex(inPos+5,drawHeaderY+5);
		ofVertex(inPos,drawHeaderY);
	ofEndShape();
	
	// draw outPos triangle
	ofBeginShape();
		ofVertex(outPos,drawHeaderY+10);
		ofVertex(outPos-5,drawHeaderY+5);
		ofVertex(outPos,drawHeaderY);
	ofEndShape();
	
	// draw header line
    ofSetLineWidth(1);
	ofLine(originXAtEnd-headerPos+1,drawHeaderY+50,originXAtEnd-headerPos,drawHeaderY-40);
	if(playing) ofSetColor(0,200,0);
	else ofSetColor(255,128,0);
	ofRect(originXAtEnd-headerPos-offsetFrames,drawHeaderY+50,oneLength,10);
	ofDrawBitmapString(ofToString(buffer->size()-currentPos)+"/"+ofToString(currentPos+1),ofPoint(originXAtEnd-headerPos-offsetFrames-(oneLength/2),drawHeaderY+45));

	// textual
	ofSetColor(255,128,0);
	ofDrawBitmapString("< " + ofToString(inFrame+1),ofPoint(inPos+0,drawHeaderY+30));
	ofDrawBitmapString(ofToString(outFrame+1) + " >" ,ofPoint(outPos-30,drawHeaderY+15));
	ofDrawBitmapString("" +ofToString(this->getLengthFrames()) + "" ,ofPoint(((outPos-inPos)/2)+inPos-30,drawHeaderY-5));
	
	ofSetColor(255,255,255);
	
	ofDisableAlphaBlending();
}

//------------------------------------------------------
float PmPdVideoHeader::getAttack(){
    return attack;
}
//------------------------------------------------------
float PmPdVideoHeader::getDecay(){
    return decay;
}
	
//------------------------------------------------------
void PmPdVideoHeader::setAttack(float f){
	attack=f;
}
//------------------------------------------------------
void PmPdVideoHeader::setDecay(float f){
	decay=f;
}

}
