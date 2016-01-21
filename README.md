# Encounter - Interactive Content

## Project Overview:

Project encounter (PE) is a solution for semi-public screens to curate and visualize content that is selected and triggered through certain sensory parameters such as location and environmental awareness of the semi-public screen.  
This version of PE uses pre-selected content from the [Nederlands Instituut voor Beeld en Geluid] (http://www.beeldengeluid.nl/). Content is selected on the basis of certain metatags that relate to the objects or locations of interest to the screen. At the moment a python script transverses through the database of open source content and returns a list of videos of interest. For the first version we have hand-picked the videos from the returned list and created our own file storage structure.

There are two software modules currently in implementation that comprise of PE:

1. OpenEncounters: This is the Openframeworks module responsible for visualizing the content. The video content is accessed and displayed in calculated timed windows and sequenced in response to certain triggers. The triggering mechanism is implemented in the other module that Encounter# provides. 

2. Encounter#/ Twitter_twitinvi : This is a framework developed in C# that is used to model semi-public screens with their points of interest (subjects for content generation) and related sensory modules. Classes used in this framework are:

	i.	**Landing Zone:** This represents the physical space that the screen can be observed from. This object is used to model attributes that directly monitor physical or reality based aspects of the landing zone, such as the time of day. It has one class member function at the moment which returns a constant corresponding to it being day or night.

	ii.	**Landmark:** This represents a landmark in an urban environment which acts as a point of interest for content generation. A landmark object type has attributes that are stored in the class members such as:

		a.	Name
		b.	Thematic_metatags: These are metatags that relate to the landmarks theme. These are used to further curate content from databases.
		c.	Hashtag_trackers: These are keywords that represent hashtags that are used to monitor twitter feeds.
		d.	Categories: These are the names of the categories that are used to classify hooks that are placed as text on top of videos.
        
	iii. **Sensors:** The intention of this was to provide a wrapper class or base class for sensor data of any kind. At the moment it contains static member functions for sensing weather conditions. In the future its derivatives can be used to add sensor objects to varying landing zones.  

## Brief overview of working: 

### OpenEncounters: 
Is written on Openframeworks version 8.4 with certain external libraries used:

### External libraries:
ParsingXML: A custom xml parser and reader that provides for efficient and fast data reading. 
ofxTextBlock : A community sourced library used to visualize text on the openframeworks canvas with font manipulation and animation effect options

Functionality can be summarized as follows:
	i.	To play video content superimposed with a text layer called a hook
	ii.	To detect a triggered event from the Encounter# module (such as a tweet) 
	iii.	To sequence a series of video-hook pairs in a pipeline
	iv.	Time keeping for each video-hook pair 

OpenEncounters accesses the content from a local file store. 

Encounter#:
Functionality:
i.	Monitors the twitter feed based for mention of hashtag_trackers for each landmark
ii.	Once a tweet is detected it starts filtering out hooks that are relevant to the landmark.
iii.	Another filteration process takes place right after which filters the hooks on the basis of sensor values, in this version the sensed 
iv.	All detected parameters such as the name of the landmark, the hooks and other attributes are written to the files that act as the interface with OpenEncounters.

## Steps for installation:
**IMPORTANT:** PE follows a strict file storage hierarchy. This must be maintained to ensure communication between the two modules and access to the content database. Filepaths that point towards the configuration files and file interfaces are hardcoded with respect to the root folder where the entire file base must be stored. Similarly, access to the content folder is also hardcoded. Thus file hierarchy must be maintained in this release.

### Visual Studio Installation
For installation of PE we must download visual studio express 2012 that runs instances of both OpenFrameworks and Encounter#. No installation package exists at the moment that configures the root folder path, thus the root folder path must be manually edited through opening the editor and the executable must be built.

http://www.microsoft.com/en-in/download/details.aspx?id=34673

### GitHub download
Once visual studio express has been downloaded, the path to the root folder must be modified in regard to where the folders have been stored.

	1)	Create a folder called "Project Encounter" on your Desktop. Case Sensitive and ensure the space between the two words

	2)	Copy the four folders from Github (Encounter#, Package\Config_interfaces, ParsingXML, of_v0.8.4_vs_release) into the folder called "Project Encounter"

 	- Open the openframeworks module solution file in VS2012 express	  ..\Desktop\ProjectEncounter\of_v0.8.4_vs_release\apps\myApps\OpenEncounters\OpenEncounters.sln
	- Open ofApp.cpp
	- In function void ofApp::setup() 
	- Change variable rootdirectory to the relevant rootdirectory that your folder "Project Encounter" exists

	3)	Open the solution file Twitter_twitinvi.sln under:
	 ..\Desktop\Project Encounter\Encounter#\Twitter_twitinvi\
	- In the file Program.cs change the rootdirectory variable to the relevant one   as done with the Openframeworks module. 

### Usage: 
Run the applications using visual studio 2012 express. 
You can run them in any order, i.e. either one can be run first. 
*	Before running the openframeworks module you will have to configure the right screen resolution that is appropriate for your screen. The function that sets the value for the screen resolution is called ofSetupOpenGL. It is present in the main.cpp:
ofSetupOpenGL(1600,900,OF_WINDOW); 
Change the parameters that represent the resolution.
*	The twitter_tweetinvi module runs in a console window where you can read the detected tweets based on the selected hashtags. Its functionality can be monitored and debugged using this console.
 
### Explanation of file structure
The file system in use for the Encounter#/ Twitter_tweetinvi application:
```
1)	JSON Configuration file for using a twitter account for the application
C:\\Users\\nikhil\\Desktop\\MediaLabs\\Sprint 4\\Twitter_context_profile.json
	2)	Excel worksheet with all listed landmarks and hooks (Database)
C:\Users\nikhil\Desktop\MediaLabs\FINALITY\Landmarks_Categories.xlsx
3)	Text file that acts as the interface between the openframeworks and twitter module
C:\Users\nikhil\Desktop\MediaLabs\FINALITY\TriggeredLandmark.txt
4)	XML file that is dynamically generated in correspondance to the triggered landmark
C:\\Users\\nikhil\\Desktop\\of_v0.8.4_vs_release\\apps\\myApps\\OpenEncounters\\bin\\data\\Hooks.xml
```

File system for Openframeworks:
1) XML file for importing all the base hooks 
C:\\Users\\nikhil\\Desktop\\of_v0.8.4_vs_release\\apps\\myApps\\OpenEncounters\\bin\\data\\BaseHooks.xml
2) XML file for importing all dynamically triggered hooks
C:\\Users\\nikhil\\Desktop\\of_v0.8.4_vs_release\\apps\\myApps\\OpenEncounters\\bin\\data\\Hooks.xml

### Content folders:

#### Content videos:
C:\Users\nikhil\Desktop\of_v0.8.4_vs_release\apps\myApps\OpenEncounters\bin\data\mediacontainer
Transition movies : C:\Users\nikhil\Desktop\of_v0.8.4_vs_release\apps\myApps\OpenEncounters\bin\data\mediacontainer\TransitionAnimations
Fonts: C:\Users\nikhil\Desktop\of_v0.8.4_vs_release\apps\myApps\OpenEncounters\bin\data\
mediacontainer//TransitionAnimations//general_transitionanimation.mov



### Known Issues:
There is a known issue with twitter_tweetinvi, an unhandled null pointer exception within one of the sensor modules. The next version will contain a fix for this. 

### Further Work:
Automating the selection of open source content while ensuring relevancy. 
The modular framework for Encounter# can be modified to model other sensors that can be added to the landing zone.

The sensor framework can be modified to separate out physical sensors, social media interfaces, etc.

We can implement methods that can interface with open source content directly using commonly used protocols.

Another parent class can be added for landing zone, which could act as the location for where the screens are implemented. This goes with the logic of there being a possibility of multiple screens in one location.
The application can be integrated with a cloud based IOT platform that can keep a memory of the activity sensed around the screen by the solution. We can further provide analytics specific to the application, requirement and vision for using the solution.
