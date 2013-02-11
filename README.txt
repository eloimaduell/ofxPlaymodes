Playmodes 3 
////////////
Audiovisual Sampler Engine by Arturo Castro + www.playmodes.com.

A new approach to ofxPlaymodes, this time all the audio part is processed by an Pd (Pure Data) patch which handles all the audio shit.

Still in development ! Find the bugs !!

info@playmodes.com

* example_PM3 : for testing the graphic pipeline (openGL)
* example_PM3_PD : for testing the audiovisual pipeline (openGL + Pdata)

CHANGES 
///////
* VideoFrames are saved as Pixels and just converted to ofTexture on draw. For such change, we introduced ofPBO and ofFastFBORead.