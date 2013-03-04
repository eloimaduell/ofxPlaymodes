#ifndef _PMTHREADEDOSCSENDER_OBJECT
#define _PMTHREADEDOSCSENDER_OBJECT
 
#include "ofMain.h"
#include "ofEvents.h"
#include "ofxOsc.h"

typedef struct
{
	float	value;
	string	address;
} pmOscMessage;

class pmThreadedOscSender : public ofThread, public ofxOscSender
{

	public:
		
	pmThreadedOscSender();
	~pmThreadedOscSender();

	int							oscSendPort;
		
	void setup(string ip,int _oscPort);
	void stop();
	void threadedFunction();
	void setThreadSleepMs(int ms);
	void setVerbose(bool b);
	void draw();
	
	void pmSendMessage(pmOscMessage &f);
	ofEvent<pmOscMessage>		newOscMessage;

	private:
	
	int				oscPort;
	string			oscIP;
	int				threadSleepMs;
	bool			verbose;
	
	
};

#endif
