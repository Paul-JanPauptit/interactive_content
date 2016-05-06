
#include "ofApp.h"
#include "ofUtils.h"
#include "ofImage.h"
#include "ofTypes.h"
#include "ofGraphics.h"
#include "ofAppRunner.h"
#include "ofFileUtils.h"
#include "Poco/String.h"
#include "Poco/LocalDateTime.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/URI.h"
#include <cctype>
#include <windows.data.json.h>
#include <windows.h>
#include <stdio.h>
#include <ctime>


// Method to check the triggered landmark from text file that the twitter app writes to
TriggeredLandmark ofApp::Check_triggered_landmark ()
{
	// Opening text file that is used for the interface between OF and Encounter
	ifstream myfile (rootdirectory + "\\Package\\Config_Interfaces\\TriggeredLandmark.txt");

	std::vector<std::string> lines;

	string buffer;
	if (myfile.is_open())
	{
		while ( getline (myfile, buffer) )
		{
			lines.push_back(buffer.c_str());
		}
	}

	TriggeredLandmark landmark;
	if (lines.size() > 0)
		landmark.name = lines[0];
	if (lines.size() > 1)
		landmark.hashtag = lines[1];
	return landmark;
}

// Class hook default constructor
ofApp::HookFromEncouter::HookFromEncouter()
{
	text = "Would you befriend an elf? ";
	hookfont = "COPRGTB.ttf";
}

// Gets hooks by parsing xml file filename, reading in node values and returning a vector of HookFromEncounter objects
vector<ofApp::HookFromEncouter> ofApp::GetHooks(string filename)
{
	vector<HookFromEncouter> hooks_dynamic_import;

	string text;
	string font;

	//Getting data from XML parser
	vector<Hook> hooksfromxml = GetData(filename);
	HookFromEncouter hookTemp;
	
	// Object conversion
	for (int i = 0; i < hooksfromxml.size() ; i++)
	{
		text.clear();
		font.clear();
		text = hooksfromxml.at(i).text;
		font = hooksfromxml.at(i).hookfont;
		
		hookTemp.text = text;
		hookTemp.hookfont = font;

		hooks_dynamic_import.push_back(hookTemp);
	}

	return hooks_dynamic_import;
}


// OF REQUIREMENTS
// OF has to handle video placement, playback and sequencing 
// OF has to handle Hook placement, sequencing and timing
// OF has to handle visual blocks with animated elements 

//--------------------------------------------------------------
void ofApp::setup() {

	rootdirectory = ofFilePath::getCurrentExeDir() + "..\\..\\..\\..\\..\\";

	// MODULE: PIPELINE VARIABLES AND THREADS

	videoCountPipeline = 0;
	generic_count = 0;
	hookindex = 0;
	hookcycle = 0;

	// MODULE: Time and condition triggers for various visuals

	Time_LandmarkWindow = 120;
	Time_PerVideo = 30;
	Time_Transition_Video = 8; 
	Time_PerHook = 10;

	firstcycle = true;
	landmark_windowlive = false;
	nolandmarkwindowlive = false;
	transition_movie_loaded = false;

	landmarks[0]= "Volkshotel";
	landmarks[1]= "Oba";
	landmarks[2]= "Artis";
	landmarks[3]= "Wibautstraat";
	landmarks[4]= "Oosterpark";
	landmarks[5]= "Sciencepark";

	// Testing the video triggered by diplaying its path, can be set on in the draw function
	TempVideoPath.loadFont("COPRGTB.ttf", 32);

	// Getting all hooks from base xml file that contains all the hooks
	hooks_base_import = ofApp::GetHooks( rootdirectory + "of_v0.8.4_vs_release\\apps\\myApps\\OpenEncounters\\bin\\data\\BaseHooks.xml");

	fontsize = Global::FONT_SIZE;
	// ofToggleFullscreen();
}


void ofApp::playNextMovie(string landmark)
{
	string path = rootdirectory;
	path.append("of_v0.8.4_vs_release\\apps\\myApps\\OpenEncounters\\bin\\data\\MediaContainer\\").append(landmark);

	ofDirectory dir;
	if (videoDirs.count(path) == 0)
	{
		dir = ofDirectory(path);
		dir.allowExt("mp4");
		dir.listDir();
		videoDirs[path] = dir;
	}
	else
		dir = videoDirs[path];

	int count = dir.size();
	if (videoCountPipeline >= count)
		videoCountPipeline = 0;

	string filename = dir.getPath(videoCountPipeline);
	currentVideoContainer.loadMovie(filename);
	videoCountPipeline++;
}

//--------------------------------------------------------------
void ofApp::update(){
	
	// Reading textfile interface and finding the latest landmark triggered	
	TriggeredLandmark triggeredLandMark = Check_triggered_landmark();
	Landmark_current = triggeredLandMark.name;

	// CONDITIONAL MODULE 1 : FIRST CYCLE
	if (firstcycle == true)
	{
		ClockStart_LandmarkWindow = std::clock();
		ClockStart_VideoWindow=std::clock();
		ClockStart_Hook = std::clock();

		if (Landmark_current.size() != NULL)
		{
			// QUEUE NEXT BLOCK : LANDMARK AND HOOK
			Landmarks_queue.push(triggeredLandMark);
			hooks_dynamic_import = ofApp::GetHooks(rootdirectory + "of_v0.8.4_vs_release\\apps\\myApps\\OpenEncounters\\bin\\data\\Hooks.xml");
			Hooks_queue.push(hooks_dynamic_import);
			ofApp::HookFromEncouter hook =  Hooks_queue.front().at(hookindex);
			HookText = hook.text;
			// Setting text, font and dropshadow
			HookTextHolder.init(hook.hookfont, fontsize);
			HookDropShadow.init(hook.hookfont, fontsize);
			hookindex++;
			HashtagTextHolder.init(hook.hookfont, fontsize * 0.8f);
			HashtagDropShadow.init(hook.hookfont, fontsize * 0.8f);
			HashtagTextHolder.setText(triggeredLandMark.hashtag);
			HashtagDropShadow.setText(triggeredLandMark.hashtag);

			playNextMovie(Landmark_current);
			landmark_windowlive = true;
		}

		if (Landmark_current.size() == NULL)
		{
			videofrompipeline.clear();
			generic_count = rand() % 5;
			string LandmarkinQueue = landmarks[generic_count];
			playNextMovie(Landmark_current);
			TextHolder = videofrompipeline;
			// Setting text, font and dropshadow
			HookText = hooks_base_import.at(hookindex).text;
			HookTextHolder.init(hooks_base_import.at(hookindex).hookfont, fontsize);
			HookDropShadow.init(hooks_base_import.at(hookindex).hookfont, fontsize);
			hookindex++;
			landmark_windowlive = true;
		}
	}

	// MODULE: TIMERS
	Timer_LandmarkWindow = ( std::clock() - ClockStart_LandmarkWindow ) / (double) CLOCKS_PER_SEC;
	Timer_VideoWindow = ( std::clock() - ClockStart_VideoWindow ) / (double) CLOCKS_PER_SEC;
	Timer_TransitionMapVideo = ( std::clock() - ClockStart_TransitionMapVideo ) / (double) CLOCKS_PER_SEC;
	Timer_Hook = ( std::clock() - ClockStart_Hook ) / (double) CLOCKS_PER_SEC;

	// CONDITIONAL_MODULE 2: NO LANDMARK TRIGGERED
	if (Landmark_current.size() == NULL || Landmarks_queue.size() == 0 || nolandmarkwindowlive == true)
	{
		nolandmarkwindowlive = true;

		if (Timer_Hook >= Time_PerHook)
		{
			// Coming here from the first cycle block
			// Getting hook text from the base hooks vector which contains all the hooks

			// Hooks only show at even cycles because 
			if (hookcycle % 2 == 0)
			{
				HookText.clear();
			}

			if (hookcycle % 2 != 0)
			{
				HookText.clear();
				HookTextHolder.init(hooks_base_import.at(hookindex).hookfont, fontsize);
				HookDropShadow.init(hooks_base_import.at(hookindex).hookfont, fontsize);
				HookText = hooks_base_import.at(hookindex).text;
				
				//HookTextHolder.loadFont(hooks_base_import.at(hookindex).hookfont, 32);
				hookindex++;

			}

			// Resetting the hook timer
			ClockStart_Hook = std::clock();
			hookcycle++;
		}		

		if ( Timer_VideoWindow >= Time_PerVideo )
		{
			videofrompipeline.clear();
			playNextMovie(landmarks[generic_count]);
			videoCountPipeline = rand() % 3;
			TextHolder = videofrompipeline;

			// HashtagTextHolder.init("", fontsize);

			// Reset video clock
			ClockStart_VideoWindow = std::clock();
			generic_count = rand() % 5;

		}

		if (videoCountPipeline == numberOfvideosinpipeline)
		{
			videoCountPipeline = 0;
			generic_count = 0;
		}

	}

	// CONDITIONAL_MODULE 3: LANDMARK TRIGGERED
	if (Landmark_current.size() != NULL && nolandmarkwindowlive != true)
	{
		// LANDMARK WINDOW LIVE
		if (Timer_LandmarkWindow <= Time_LandmarkWindow)
		{
			if (Timer_Hook >= Time_PerHook)
			{
				if (hookcycle % 2 == 0)
				{
					HookText.clear();
				}
				
				if (hookcycle % 2 != 0)
				{
					HookText.clear();
					HookText = Hooks_queue.front().at(hookindex).text;
					HookTextHolder.init(Hooks_queue.front().at(hookindex).hookfont, fontsize);
					HookDropShadow.init(Hooks_queue.front().at(hookindex).hookfont, fontsize);
					hookcount = Hooks_queue.front().size();
					hookindex = rand() % hookcount;
					// Increment the index so next hook is read in from the vector in the next cycle
					//hookindex++;
				}
				ClockStart_Hook= std::clock();
				hookcycle++;

			}

			if ( Timer_VideoWindow >= Time_PerVideo )
			{
				// Set the path for video to be played based on the current landmark in queue and the video count of the mediacontainer folder
				videofrompipeline.clear();
				TriggeredLandmark queuedLandmark = Landmarks_queue.front();
				playNextMovie(queuedLandmark.name);
				// Reset video clock
				ClockStart_VideoWindow = std::clock();
			
				HashtagTextHolder.setText(queuedLandmark.hashtag);
				HashtagDropShadow.setText(queuedLandmark.hashtag);

				// TEST: To check if the right video is being played
				TextHolder = videofrompipeline;
			}

		}
	}

	// LANDMARK WINDOW TIMED OUT
	if (Timer_LandmarkWindow >= Time_LandmarkWindow)
	{
		// TRANSITION ANIMATIONS

		// No need for hooks
		HookText = "";

		// Pop the landmark that has been executed out of the queue

		if (transition_movie_loaded == false)
		{
			if(nolandmarkwindowlive == true)
			{
				currentVideoContainer.loadMovie("mediacontainer//TransitionAnimations//general_transitionanimation.mov");
			}	

			else 
			{
				string queuedLandmark = Landmarks_queue.front().name;
				currentVideoContainer.loadMovie("mediacontainer//TransitionAnimations//" + queuedLandmark + "_transitionanimation.mov");
			}	

			ClockStart_TransitionMapVideo = std::clock();
			transition_movie_loaded = true;
			Timer_TransitionMapVideo = ( std::clock() - ClockStart_TransitionMapVideo ) / (double) CLOCKS_PER_SEC;
		}   

		if (Timer_TransitionMapVideo <= Time_Transition_Video)// Transition map is sequence is not over)
		{
			// Keep updating the Transition movie				
		}

		else if (Timer_TransitionMapVideo >= Time_Transition_Video)
		{
			// ENDING OF THE LANDMARK WINDOW
			ClockStart_LandmarkWindow = std::clock();
			transition_movie_loaded = false;
			if(Landmarks_queue.size() != 0 && nolandmarkwindowlive != true)
			{
				Landmarks_queue.pop();
				Hooks_queue.pop();
			}				

			//Resetting various counts
			videoCountPipeline = 0;
			hookindex = 0;
			hookcycle = 0;

			nolandmarkwindowlive = false;
		}

	}

	// CONDITIONAL_MODULE 4: NEW LANDMARK IN PIPELINE
	if (firstcycle != true){
		if(Landmark_current != Landmark_previous)
		{			
			Landmarks_queue.push(triggeredLandMark);
			hooks_dynamic_import = ofApp::GetHooks(rootdirectory + "of_v0.8.4_vs_release\\apps\\myApps\\OpenEncounters\\bin\\data\\Hooks.xml");
			Hooks_queue.push(hooks_dynamic_import);
		}
	}

	// Landmark change tracker
	Landmark_previous = Landmark_current;
	
	// MODULE: VIDEO FRAME UPDATE 

	currentVideoContainer.update();
	HookTextHolder.setText(HookText);
	HookDropShadow.setText(HookText);

	//Wrapping text to a defined width of pixels
	wrap = HookTextHolder.wrapTextX(0.8 * Global::VIDEO_RESOLUTION_X);
	wrapdropshadow = HookDropShadow.wrapTextX(0.8 * Global::VIDEO_RESOLUTION_X);
		
	// Modifying color on each word in hook
	if (hookcycle % 2 == 0)
	{
		for(int i = 0; i < HookTextHolder.words.size(); i++)
		{
			if(i % 2 == 0)
			{
				HookTextHolder.words.at(i).color.r = 255;
				HookTextHolder.words.at(i).color.g = 255;
				HookTextHolder.words.at(i).color.b = 153;
			}
		}
	}
	if (hookcycle % 2 == 0)
	{
		for(int i = 0; i < HookDropShadow.words.size(); i++)
		{
			if(i % 2 == 0)
			{
				HookDropShadow.words.at(i).color.r = 8;
				HookDropShadow.words.at(i).color.g = 8;
				HookDropShadow.words.at(i).color.b = 8;
				//HookDropShadow.words.at(i).color.a = 0.5;
			}
		}
	}

	HashtagTextHolder.setColor(255, 255, 153, 255);
	HashtagDropShadow.setColor(8, 8, 8, 255);

	firstcycle = false;
	
}

void ofApp::draw(){

	// Drawing video frame
	ofSetColor(255,255,255);
	currentVideoContainer.draw(0,0, Global::VIDEO_RESOLUTION_X, Global::VIDEO_RESOLUTION_Y);

	// Video path test string : uncomment for testing purposes
	//TempVideoPath.drawString(TextHolder, 50, 50);

	// Drawing Hook
	HookDropShadow.drawCenter(Global::VIDEO_RESOLUTION_X / 2 + 5, Global::VIDEO_RESOLUTION_Y / 2 - 100 + 5);
	HookTextHolder.drawCenter(Global::VIDEO_RESOLUTION_X / 2, Global::VIDEO_RESOLUTION_Y / 2 - 100);

	// Drawing hashtag (but only if we are showing a hook)
	if (!HookText.empty())
	{
		HashtagDropShadow.drawCenter(Global::VIDEO_RESOLUTION_X / 2 + 5, 10 + 5);
		HashtagTextHolder.drawCenter(Global::VIDEO_RESOLUTION_X / 2, 10);
	}
	
	
	// Playing the video frame
	currentVideoContainer.play();

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
