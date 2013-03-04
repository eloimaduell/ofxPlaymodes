#ifndef _PMTHREADEDOSCRECEIVER_OBJECT
#define _PMTHREADEDOSCRECEIVER_OBJECT
 
#include "ofMain.h"
#include "ofEvents.h"
#include "ofxOsc.h"

typedef struct
{
	float	value;
	string	address;
} pmOscMessage;

class pmThreadedOscReceiver : public ofThread, public ofxOscReceiver
{

	public:
		
	pmThreadedOscReceiver();
	~pmThreadedOscReceiver();

	int							oscReceivePort;
	
	ofEvent<pmOscMessage>		newOscMessage;
		
	void setup(int _oscPort, string _filter="");
	void stop();
	void threadedFunction();
	void setAddressFilter(string s);
	void setThreadSleepMs(int ms);
	void setVerbose(bool b);
	void draw();
	
		

	private:
	
	int				oscPort;
	string			filterAddress;
	int				threadSleepMs;
	bool			verbose;
	
};

#endif
