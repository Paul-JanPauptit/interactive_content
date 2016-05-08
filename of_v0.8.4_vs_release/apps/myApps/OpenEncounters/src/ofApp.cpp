
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

	videoCountPipeline = -1;
	landmark_index = 0;
	hookindex = 0;
	hookcycle = 0;

	// MODULE: Time and condition triggers for various visuals

	Time_LandmarkWindow = 120;
	Time_PerVideo = 30;
	Time_Transition_Video = 8; 
	Time_PerHook = 10;
	Time_PerAnnouncement = 10;

	firstcycle = true;
	landmark_windowlive = false;
	nolandmarkwindowlive = false;
	transition_initialized = false;

	// Create an array of landmark folders to fallback on when no twitter input is received. 
	string path = rootdirectory;
	path.append("of_v0.8.4_vs_release\\apps\\myApps\\OpenEncounters\\bin\\data\\MediaContainer\\");
	ofDirectory dir = ofDirectory(path);
	dir.allowExt("");
	int size = dir.listDir();
	for (int i = 0; i < size; i++)
		landmarks.push_back(dir.getName(i));

	// Testing the video triggered by diplaying its path, can be set on in the draw function
	TempVideoPath.loadFont("COPRGTB.ttf", 32);

	// Getting all hooks from base xml file that contains all the hooks
	hooks_base_import = ofApp::GetHooks( rootdirectory + "of_v0.8.4_vs_release\\apps\\myApps\\OpenEncounters\\bin\\data\\BaseHooks.xml");

	fontsize = Global::FONT_SIZE;
	// ofToggleFullscreen();

	// Background color (only visible if no movie is playing)
	ofBackground(50, 50, 58, 255);
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

	// Initially, pick a random video
	if (videoCountPipeline = -1)
		videoCountPipeline = rand() % count;

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

			playNextMovie(Landmark_current);
			landmark_windowlive = true;
		}

		if (Landmark_current.size() == NULL)
		{
			videofrompipeline.clear();
			landmark_index = GetRandomLandmarkIndex();
			string LandmarkinQueue = landmarks[landmark_index];
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
	if (!transition_initialized && (Landmark_current.size() == NULL || Landmarks_queue.size() == 0 || nolandmarkwindowlive == true))
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
			landmark_index = GetRandomLandmarkIndex();
			playNextMovie(landmarks[landmark_index]);
			TextHolder = videofrompipeline;

			// Reset video clock
			ClockStart_VideoWindow = std::clock();
		}

		if (videoCountPipeline == numberOfvideosinpipeline)
		{
			videoCountPipeline = 0;
			landmark_index = 0;
		}

	}

	// CONDITIONAL_MODULE 3: LANDMARK TRIGGERED
	if (!transition_initialized && (Landmark_current.size() != NULL && nolandmarkwindowlive != true))
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

				// TEST: To check if the right video is being played
				TextHolder = videofrompipeline;
			}

		}
	}

	// LANDMARK WINDOW NEARING END - announce next landmark
	if (Timer_LandmarkWindow >= Time_LandmarkWindow - Time_PerAnnouncement && !transition_initialized)
	{
		if (HashtagTextHolder.rawText.empty())
		{
			// Initialize next landmark: either from queue or random
			string queuedLandmark;
			if (Landmarks_queue.size() > 0) 
			{
				TriggeredLandmark landmark = Landmarks_queue.front();
				Landmark_next = landmark.name;
				string announcement = "Up next: " + landmark.hashtag;
				HashtagTextHolder.setText(announcement);
				HashtagDropShadow.setText(announcement);
			}
			else
				Landmark_next = landmarks[GetRandomLandmarkIndex()];
		}
	}

	// LANDMARK WINDOW TIMED OUT - transition and landmark cleanup
	if (Timer_LandmarkWindow >= Time_LandmarkWindow)
	{

		if (transition_initialized == false)
		{


			if (Global::SUPPORTS_TRANSITION_MOVIES)
			{
				// No need for hooks
				HookText = "";


				// TRANSITION ANIMATIONS
				if (nolandmarkwindowlive == true)
				{
					currentVideoContainer.loadMovie("mediacontainer//TransitionAnimations//general_transitionanimation.mov");
				}	

				else 
				{
					currentVideoContainer.loadMovie("mediacontainer//TransitionAnimations//" + Landmark_next + "_transitionanimation.mov");
				}
			}
			else
			{
				// Simple transition - Blank screen with name of next landmark
				currentVideoContainer.stop();
				//currentVideoContainer.close();
				HookText = Landmark_next;
			}

			HashtagTextHolder.setText("");
			HashtagDropShadow.setText("");

			ClockStart_TransitionMapVideo = std::clock();
			transition_initialized = true;
			Timer_TransitionMapVideo = ( std::clock() - ClockStart_TransitionMapVideo ) / (double) CLOCKS_PER_SEC;
		}

		if (Timer_TransitionMapVideo <= Time_Transition_Video) // Transition map is sequence is not over
		{
			// Keep updating the Transition movie				
		}
		else if (Timer_TransitionMapVideo >= Time_Transition_Video)
		{
			// ENDING OF THE LANDMARK WINDOW
			ClockStart_LandmarkWindow = std::clock();
			transition_initialized = false;
			if(Landmarks_queue.size() != 0 && nolandmarkwindowlive != true)
			{
				Landmarks_queue.pop();
				Hooks_queue.pop();
			}				

			//Resetting various counts
			videoCountPipeline = 0;
			hookindex = 0;
			hookcycle = 0;

			HookText.clear();
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
	
	HookTextHolder.setColor(255, 255, 153, 255);
	HookDropShadow.setColor(8, 8, 8, 255);

	HashtagTextHolder.setColor(255, 255, 153, 255);
	HashtagDropShadow.setColor(8, 8, 8, 255);

	firstcycle = false;
	
}

void ofApp::draw(){

	// Drawing and advancing video frame
	ofSetColor(255,255,255);

	// Drawing and updating  the video frame, unless we are playing a transition without a background movie
	if (Global::SUPPORTS_TRANSITION_MOVIES || !transition_initialized)
	{
		currentVideoContainer.draw(0,0, Global::VIDEO_RESOLUTION_X, Global::VIDEO_RESOLUTION_Y);
		currentVideoContainer.play();
	}

	// Video path test string : uncomment for testing purposes
	//TempVideoPath.drawString(TextHolder, 50, 50);

	// Drawing Hook
	HookDropShadow.drawCenter(Global::VIDEO_RESOLUTION_X / 2 + Global::TEXT_SHADOW_OFFSET, Global::VIDEO_RESOLUTION_Y / 2 - 100 + Global::TEXT_SHADOW_OFFSET);
	HookTextHolder.drawCenter(Global::VIDEO_RESOLUTION_X / 2, Global::VIDEO_RESOLUTION_Y / 2 - 100);

	// Drawing hashtag (but only if we are showing a hook)
	HashtagDropShadow.drawCenter(Global::VIDEO_RESOLUTION_X / 2 + Global::TEXT_SHADOW_OFFSET, 10 + Global::TEXT_SHADOW_OFFSET);
	HashtagTextHolder.drawCenter(Global::VIDEO_RESOLUTION_X / 2, 10);
}

//--------------------------------------------------------------
int ofApp::GetRandomLandmarkIndex()
{
	// Returns a random landmark, different from the current
	int size = landmarks.size();
	int index = rand() % (size - 1);
	int currentIndex = 0;
	for (int currendex = 0; currentIndex < size; currentIndex++)
	{
		if (landmarks[currentIndex] == Landmark_current)
		{
			break;
		}
	}
	if (index >= currentIndex)
	{
		index++;
	}
	return index;
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
