#pragma once

#include "ofMain.h"

#include <cstdio>
#include <fstream>
#include <iostream>
#include "ofxTextBlock.h"


#include <queue>
#include <vector>

#include "ParsingXML.h"


class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream);

		class HookFromEncouter {
		      
		public:
			std::string text;
			std::string hookfont;
			HookFromEncouter();
			HookFromEncouter(string text1, string hookfont1);
			HookFromEncouter(string landmark);
			ofApp::HookFromEncouter::HookFromEncouter(int i);
		         
		};

		/*ofxSVG window_large;
		ofxSVG window_small;
		ofxSVG window_square;*/

		ofPolyline polyline;
		ofPath path;

		ofImage background_map;
		ofImage windmill_blades;
		ofImage window_large;
		ofImage window_small;
		ofImage window_square;

		vector<ofPolyline> outlines;

		int deg;
		int y;

		ofVideoPlayer brouwerij_video;
		ofVideoPlayer tropen_video;

		
		// Other variables
		int frame;
		int s;
		int angular_rotation;

		// Twitter variables

		std::string landmark_weather_videotag;

		// Hook based mood enhancement module variables

		ofVideoPlayer landmark_weather_video;
		bool movie_loaded;

		int tweet_count;
		int tweet_count_previous;

		string buffer;

		// Landmark
		string currentLandmark;
		
		string Check_triggered_landmark();
		
		string Landmark_previous;
		string Landmark_current;


		// Videos 
		ofVideoPlayer currentVideoContainer;
		int videoCountPipeline;
		string videofrompipeline;
		
		int numberOfvideosinpipeline;

		// Timing

		std::clock_t ClockStart_LandmarkWindow;
		double Timer_LandmarkWindow;

		std::clock_t ClockStart_VideoWindow;
		double Timer_VideoWindow;

		std::clock_t ClockStart_TransitionMapVideo;
		double Timer_TransitionMapVideo;

		std::clock_t ClockStart_Hook;
		double Timer_Hook;

		// Preset time condtions
		
		int Time_LandmarkWindow;
		int Time_PerVideo;
		int Time_Transition_Video; 
		int Time_PerHook;

		int N;

		// ALL HOOK RELATED VECTORS, QUEUES AND COUNT VARIABLES AND THE POPULATE METHOD

		vector<ofApp::HookFromEncouter> hooks_base_import;
		vector<ofApp::HookFromEncouter> hooks_dynamic_import;
				
		vector<ofApp::HookFromEncouter> ofApp::GetHooks(string filename);
	
		std::queue<std::vector<ofApp::HookFromEncouter>> Hooks_queue;
		int hookindex;
		int hookcycle;
		int hookcount;
		int wrap;

		
		// ALL LANDMARK RELATED QUEUES 
		queue <string> Landmarks_queue;

		bool firstcycle;
		bool transition_movie_loaded;
		bool landmark_windowlive;
		bool nolandmarkwindowlive;

		string landmarks[6];

		ofTrueTypeFont TempVideoPath;
		string TextHolder;

		// TEXT VISUALIZATION HOLDERS -> FOR HOOKS
		ofxTextBlock HookTextHolder;
		ofxTextBlock HookDropShadow;
		string HookText;
		int fontsize;
		int wrapdropshadow;
				
		int generic_count;

		// Filesystem

		string rootdirectory;
		

};

