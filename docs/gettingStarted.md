Getting Started {#gettingStarted_page}
===============

In brief, you need a few things to use CX:
+ A reasonably modern computer with Windows or Linux.
+ A C++ compiler/IDE.
+ [openFrameworks](http://www.openframeworks.cc), which CX relies on.
+ A copy of CX, which you can get from the github repository (https://github.com/hardmanko/ofxCX).

The sections below go into a more detail about these things.

System Requirements
-------------------

The short version: Use a reasonably modern computer (made around 2010 or later) with either Windows or Linux.

The long(er) version:

Although openFrameworks works on a wide variety of hardware and software, CX does not support all of it. For example, CX does not support iPhones, although openFrameworks does.
CX currently supports computers running Windows and Linux. 
I have tried to get CX working on OSx, but openFrameworks does not support C++11 on OSx (see this thread https://github.com/openframeworks/openFrameworks/issues/2335). Once openFrameworks, C++11, and OSx work together nicely, CX should be supported on OSx.
Although, technically, openFrameworks does not support C++11 on Linux or Windows, those platforms work just fine.

As far as hardware is concerned, the minimum requirements for openFrameworks and CX are low. However, if your video card is too old, you won't be able to use some types of graphical rendering. 
Having a video card that supports OpenGL version 3.2 at least is good, although older ones will work, potentially with reduced functionality. 
Also, a 2+ core CPU is generally a good idea for psychology experiments, because one core can be hogged by CX while the operating system can use the other core for other things. Basically, use a computer made after 2010 and you will have no worries whatsoever. However, CX has been found to work with reduced functionality on computers from the mid 90's, so there is that option, although I cannot make any guarantees that it will work on any given computer of that vintage.

Compiler/IDE
------------

You will need a C++ compiler/IDE with support for C++11, because CX uses C++11 features extensively. The openFrameworks [download page](http://openframeworks.cc/download/) lists the officially supported IDEs for the different platforms. However, you can probably make openFrameworks work with other compilers.

For Windows, I primarily use Visual Studio 2012, which is well-supported by openFrameworks. Visual Studio is by far the best C++ IDE of those I have used, but the Professional version of it costs money (unless you are a student, in which it is free through Dreamspark: https://www.dreamspark.com/).
If you don't want to buy Visual Studio just to try CX, you can use Visual Studio 2012 Express (http://www.microsoft.com/en-us/download/details.aspx?id=34673), which is free but does not have all of the functionality of the full version of Visual Studio.
If you want something that is not only gratis, but also libre, you can use Code::Blocks (http://www.codeblocks.org/).

If you are using Linux, you can use [Code::Blocks](http://www.codeblocks.org/) or just figure out how to make openFrameworks work without an IDE.


Getting openFrameworks
-------------------------

In order to use CX, you must have openFrameworks installed. Currently versions 0.8.4 and 0.8.0 of openFrameworks are supported by CX.
The latest version of openFrameworks can be downloaded from [here](http://openframeworks.cc/download/) and older versions from [here](http://openframeworks.cc/download/older.html). 
The main openFrameworks download page (http://openframeworks.cc/download/) has information about how to install openFrameworks, depending on what development environment you are using.

### Linux openFramworks 0.8.0 installation notes ###
There are two issues with installing openFrameworks 0.8.0 on Linux. openFrameworks 0.8.4 does not have these issues. All directories are given relative to where you installed openFramworks.

#### Problem 1: 
For at least some Linux distributions, `scripts/linux/WHATEVER_OS/install_dependencies.sh` must be modified so as to ignore some of the gstreamer-ffmpeg stuff, 
because the script doesn't seem to properly deal with the case of gstreamer_0.1-ffmpeg not existing in the available software sources. 
A newer version of gstreamer can be installed by commenting out everything related to selecting a gstreamer version, except

    GSTREAMER_VERSION=1.0
    GSTREAMER_FFMPEG=gstreamer${GSTREAMER_VERSION}-libav

which does the trick for me. I'm not sure that 1.0 is the latest version of gstreamer, but this WORKFORME.

#### Problem 2: 
`ofTrueTypeFont.cpp` cannot compile because of some strange folder structure issue. All you need to do is 
modify `libs/openFrameworks/graphics/ofTrueTypeFont.cpp` a little. At the top of the file, there are include directives for some freetype files. They look like

	#include "freetype2/freetype/freetype.h"

and need to be changed by removing the intermediate freetype directory to

	#include "freetype2/freetype.h"

for each include of a freetype file (they are all together at the top of the file). Now running `complileOF.sh` should work.



Installing CX
-------------

Once you have installed openFrameworks, you can install CX.
First, download CX from its [github repository](https://github.com/hardmanko/ofxCX) by clicking on the "Download ZIP" button on the right (or by using git clone, if you want to be fancy).

Put the contents of the zip file into into a directory named `ofxCX` in `OFDIR/addons`, where OFDIR is where you put openFrameworks when you installed it.

You are now done installing things!


Creating Your First CX Project
------------------------------

To use CX in a project, you will use the openFrameworks project generator, so you might want to have a look at it's help page [here](http://openframeworks.cc/tutorials/introduction/002_projectGenerator.html), but it's really easy to use, so you might not need to read up on it.

1. Use the oF project generator (in `OFDIR/projectGenerator/projectGeneratorSimple.exe` for Windows) to create a new project that uses the ofxCX addon.
The project generator asks you what to name your project and allows you to change where to put it (defaults to `OFDIR/apps/myApps/`).
Once you have dont that, click the "Addons" button and check the box next to ofxCX. If ofxCX does not appear in the list of addons, you probably didn't put the 
ofxCX directory in the right place (it must be in the openFrameworks addons directory).
2. Go to the newly-created project directory (that you chose when creating the project in step 1) and go into the `src` subdirectory.
3. Delete all of the files in the src directory (main.cpp, testApp.h, and testApp.cpp). The project generator creates these files, but you don't need them for CX.
4. Create a new .cpp file in the `src` subdirectory and give it a name, like "MyFirstExperiment.cpp". In new file, you will need to include CX_EntryPoint.h and define a function called `runExperiment`, like in the example below:
\code{.cpp}
#include "CX_EntryPoint.h"

void runExperiment (void) {
	//Do everything you need to do for your experiment
}
\endcode
Inlcuding CX_EntryPoint.h brings into your program everything from CX and openFrameworks so that you can use it. 
`runExperiment` is the CX version of a `main` function: It is called once, after CX has been set up, and the program closes after `runExperiment` returns.
5. Now you need to tell the compiler that it should compile the whole project, including openFrameworks, CX, and your new .cpp file.
For Visual Studio (VS), you go to the root directory for your application (up one level from `src`) and open the file with the same name as your project with the `.sln` extension.
This should open VS and your project. On the left side of the VS window, there should be a pane called "Solution Explorer". Within the Solution Explorer, there should be a few items. One will
be called "Solution 'APP_NAME' (2 projects)", which contains your project, called APP_NAME, and a project called `openframeworksLib`. You should expand your project until you can see a folder called `src`.
It will have the same files as you deleted in step 3 listed there, so get rid of them by highlighting them and pressing the delete key (or right click on them and select "Exclude From Project").
Now right click on the `src` folder in VS and select "Add" -> "Existing item...". In the file selector that opens, navigate your way to the `src` folder in your project directory and select the
.cpp file you made in step 4. You can alternately drag and drop your cpp file onto the `src` folder in VS. 
Now press F5, or select "DEBUG" -> "Start Debugging" from the menu bar at the top of the VS window. This will compile and run your project in debug mode. It will take a long time
to compile the first time, because it has to compile all of openFrameworks and all of CX the first time. However, subsequent builds will only need to compile your code and will be much faster.



That's all you need to do to get started. However, you probably have no idea what to put into `runExperiment` at this point.
You should look at the \ref examplesAndTutorials in order to learn more about how CX works. You should start with the helloWorld example.




\section examplesAndTutorials Examples and Tutorials

There are several examples of how to use CX. The example files can be found in the CX directory (`OF_DIR/addons/ofxCX`) in subfolders with names beginning with "example-". 
Some of the examples are on a specific topic and others are sample experiments that integrate together different features of CX. 

In order to use the examples and tutorials, do everything for creating a new CX project (above) up until step 3. Then, instead of creating a new .cpp file in step 4,
copy one of the example .cpp files into the `src` directory. 

Some of the examples have data files that they need run. For example, the runderingTest example has a picture of some birds that it uses. 
If the example has data, in the example directory, there will be a directory called `bin` with a directory under it called `data` containing the necessary files.

If the example has a data folder, copy the contents of that folder into `yourProjectDirectory/bin/data`. The `bin/data` folder might not 
exist at this point. You can create it.

1. Use the oF project generator (OFDIR/projectGenerator/projectGeneratorSimple.exe) to create a new project that uses the ofxCX addon.
The project generator asks you what to name your project, where to put it (defaults to OFDIR/apps/myApps/), and has the option of selecting
addons. Click the "addons" button and check to box next to ofxCX. If ofxCX does not appear in the list of addons, you probably didn't put the 
ofxCX directory in the right place.
2. Go to the newly-created project directory (that you chose when creating the project in step 1) and go into the src subdirectory.
3. Delete all of the files in the src directory (main.cpp, testApp.h, and testApp.cpp).
4. Copy the example .cpp file into this src directory.
5. If the example has a data folder, copy the contents of that folder into yourProjectDirectory/bin/data. The bin/data folder probably won't 
exist at this point. You can create it.
6. This step depends on your compiler, but you'll need to tell it to use the example source file that you copied in step 4
when it compiles the project (and possibly to specifically not use the files you deleted from the src directory in step 3).
7. Compile and run the project.

Tutorials:
-----------------------
+ soundBuffer - Tutorial covering a number of things that you can do with CX_SoundObjects, including loading sound 
files, combining sounds, and playing them.
+ modularSynth - This tutorial demonstrates a number of ways to generate auditory stimuli using the synthesizer modules in the CX::Synth namespace.
+ dataFrame - Tutorial covering use of CX_DataFrame, which is a container for storing data of various types that is collected in an experiment.
+ logging - Tutoral explaining how the error logging system of CX works and how you can use it in your experiments.
+ animation - A simple example of a simple way to draw moving things in CX. Also includes some mouse input handling: cursor movement, clicks, and scroll wheel activity.

Experiments:
------------------------
+ changeDetection - A very straightforward change-detection task demonstrating some of the features of CX 
like presentation of time-locked stimuli, keyboard response collection, and use of the CX_RandomNumberGenerator.
There is also an advanced version of the changeDetection task that shows how to do data storage and output with a CX_DataFrame 
and how to use a custom coordinate system with visual stimuli so that you don't have to work in pixels.
+ nBack - Demonstrates advanced use of CX_SlidePresenter in the implementation of an N-Back task. An advanced version of this
example contrasts two methods of rendering stimuli with a CX_SlidePresenter, demonstrating the advantages of each.

Misc.:
-----------------------
+ helloWorld - A very basic getting started program.
+ renderingTest - Includes several examples of how to draw stuff using ofFbo (a kind 
of offscreen buffer), ofImage (for opening image files: .png, .jpg, etc.), a variety of basic oF drawing functions 
(ofCircle, ofRect, ofTriangle, etc.), and a number of CX drawing functions from the CX::Draw namespace that supplement
openFramework's drawing capabilities.