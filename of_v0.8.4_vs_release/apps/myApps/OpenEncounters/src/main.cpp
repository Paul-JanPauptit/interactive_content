#include "ofMain.h"
#include "ofApp.h"
	#include "ofAppGLFWWindow.h" 

//========================================================================
int main( ){


	ofAppGLFWWindow win;
	win.setMultiDisplayFullscreen(false);

	ofSetupOpenGL(Global::VIDEO_RESOLUTION_X,Global::VIDEO_RESOLUTION_Y,OF_WINDOW);			// <-------- setup the GL context

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp(new ofApp());

}
