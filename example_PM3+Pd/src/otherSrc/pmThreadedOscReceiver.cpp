#include "pmThreadedOscReceiver.h"

//--------------------------
pmThreadedOscReceiver::pmThreadedOscReceiver()
{
	threadSleepMs=0;
	verbose = false;
	oscPort = 0;
	filterAddress = "";
	
}                       


//--------------------------
pmThreadedOscReceiver::~pmThreadedOscReceiver()
{
	stop();
}

//--------------------------
void pmThreadedOscReceiver::setup(int _oscPort, string _filter)
{
	ofThread::startThread(true, false);   // blocking, verbose
	oscPort			= _oscPort;
	filterAddress	= _filter;
	ofxOscReceiver::setup(oscPort);	
}

//--------------------------
void pmThreadedOscReceiver::stop()
{
	stopThread();
}

//--------------------------
void pmThreadedOscReceiver::threadedFunction()
{	
	while( isThreadRunning() != 0 )
	{
		// OSC 
		// check for waiting messages
		while(ofxOscReceiver::hasWaitingMessages() )
		//if(ofxOscReceiver::hasWaitingMessages() )
		{
			// get the next message
			ofxOscMessage m;
			ofxOscReceiver::getNextMessage( &m );
			
			// check for any OSC message containing /pm3
			//if ( m.getAddress() == "/test" )
			if(m.getAddress().find(filterAddress)!=-1)
			{
				
				pmOscMessage pmM;
				pmM.address = m.getAddress();
				if(m.getArgType(0)==OFXOSC_TYPE_FLOAT)
				{
					pmM.value = m.getArgAsFloat(0);
				}
				else if (m.getArgType(0)==OFXOSC_TYPE_INT32) 
				{
					pmM.value = float (m.getArgAsInt32(0));
				}
				
				if(verbose) printf("pmThreadOsc :: new OscMessage In %s :: %f \n",pmM.address.c_str(),pmM.value);

				ofNotifyEvent(newOscMessage,pmM,this);
			}
		}

		ofSleepMillis(threadSleepMs);
	}
}

//--------------------------
void pmThreadedOscReceiver::setThreadSleepMs(int ms)
{
	threadSleepMs = ms;
}

//--------------------------
void pmThreadedOscReceiver::setVerbose(bool b)
{
	verbose = b;
}
//--------------------------
void pmThreadedOscReceiver::setAddressFilter(string s)
{
	filterAddress = s;
}
//--------------------------
void pmThreadedOscReceiver::draw()
{
	
}

	
