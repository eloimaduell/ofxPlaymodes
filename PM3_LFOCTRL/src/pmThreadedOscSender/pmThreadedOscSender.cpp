#include "pmThreadedOscSender.h"

//--------------------------
pmThreadedOscSender::pmThreadedOscSender()
{
	threadSleepMs=10;
	verbose = false;
	oscPort = 12345;
	oscIP	= "localhost";
	
}                       


//--------------------------
pmThreadedOscSender::~pmThreadedOscSender()
{
	stop();
}

//--------------------------
void pmThreadedOscSender::setup(string _ip,int _oscPort)
{
	ofThread::startThread(true, false);   // blocking, verbose
	oscPort			= _oscPort;
	oscIP			= _ip;
	ofxOscSender::setup(oscIP,oscPort);	
}

//--------------------------
void pmThreadedOscSender::stop()
{
	stopThread();
}

//--------------------------
void pmThreadedOscSender::threadedFunction()
{	
	while( isThreadRunning() != 0 )
	{
		//ofNotifyEvent(newOscMessage,pmM,this);
		ofSleepMillis(threadSleepMs);
	}
}

//--------------------------
void pmThreadedOscSender::setThreadSleepMs(int ms)
{
	threadSleepMs = ms;
}

//--------------------------
void pmThreadedOscSender::setVerbose(bool b)
{
	verbose = b;
}
//--------------------------
void pmThreadedOscSender::draw()
{
	
}

//--------------------------
void pmThreadedOscSender::pmSendMessage(pmOscMessage &f)
{
	if(verbose) printf("threadedOSCsender :: %s %d :: %s : %f\n",oscIP.c_str(),oscPort,f.address.c_str(),f.value);
	ofxOscMessage m;
	m.setAddress(f.address);
	m.addFloatArg(f.value);
	
	sendMessage(m);
	
}
	
