Playmodes 3 
////////////
Audiovisual Sampler Engine by Arturo Castro + www.playmodes.com.

A new approach to ofxPlaymodes, this time all the audio part is processed by an Pd (Pure Data) patch which handles all the audio shit.

Still in development ! Find the bugs !!

Any questions or ideas ? info@playmodes.com

* example_PM3+PD : for testing the audiovisual pipeline (openGL + Pdata)
* PM3_LFOCTRL : it's an OSC generator to trigger PM3+PD based on LFOscillators. 

CHANGES 
///////
* VideoFrames are saved as Pixels and just converted to ofTexture on draw. For such change, we introduced ofPBO and ofFastFBORead.
* Added some OSC control not yet 100% implemented just this :

	// Floats 
	//------------------------------
	/pm3/header/delay		in Ms.
	/pm3/header/in			in Ms.
	/pm3/header/length		in Ms.
	/pm3/header/speed		-16 to 16
	/pm3/master/out			0.0 to 1.0
	/pm3/master/in			0.0 to 1.0
	/pm3/buffer/feedback		0.0 to 1.0
	/pm3/buffer/scratchDecayTime	in Ms.
	
	// Floats as Booleans
	// 0.0 is false
	// 1.0 is true
	//------------------------------

	/pm3/header/play
	/pm3/header/rec
	/pm3/header/enableTimeStretch
	/pm3/header/keepWholeGrain
