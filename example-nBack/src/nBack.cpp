#include "CX_EntryPoint.h"

#include "CX_ContinuousSlidePresenter.h"

/*
This example shows how to implement an N-Back task using a CX_ContinuousSlidePresenter (CSP).
The CSP is a lot like a CX_SlidePresenter, except that it is designed to ask the user for more
slide to present every time it gets to the end of the slides that it has. It also deallocates the
video memory from the framebuffers of slides as soon as they are presented. The idea with the CSP
is that it can be used to present a long series of stimuli that all must be synchronized without
using up too many memory resources at once.

In order to use a CSP, the user must provide a function
*/

CX_ContinuousSlidePresenter csp;

CX_DataFrame df;
CX_DataFrame::rowIndex_t trialNumber = 0;
int trialCount = 40;
int lastSlideIndex = 0;
int nBack = 2;

ofTrueTypeFont letterFont;
ofTrueTypeFont instructionFont;

ofColor backgroundColor(50);
ofColor textColor(255);

char targetKey = 'f';
char nonTargetKey = 'j';

CX_Millis stimulusPresentationDuration = 1000.0;
CX_Millis interStimulusInterval = 1000.0;

void lastSlideFunction (CX_CSPInfo_t& info);
void drawStimulusForTrial (unsigned int trial, bool showInstructions);
void generateTrials (int numberOfTrials);

void setupExperiment (void) {

	Input.setup(true, false);

	letterFont.loadFont(OF_TTF_SANS, 20);
	instructionFont.loadFont(OF_TTF_SANS, 12);

	generateTrials(10);

	csp.setup(&Display);

	//Set a function that you want to be called every time the slide presenter has started to present the last
	//slide you put in. In your function, you can add more slides to the slide presenter. Every time it reaches 
	//the last slide, it will called lastSlideFunction again.
	csp.setUserFunction(&lastSlideFunction);

	//Start loading slides into the slide presenter. Load up a little countdown screen.
	for (int i = 3; i > 0; i--) {
		csp.beginDrawingNextSlide(1000000, "fixation");
		ofBackground(backgroundColor);
		ofSetColor(textColor);
		stringstream s;
		s << nBack << "-back task" << endl;
		s << "Press '" << targetKey << "' for targets and '" << nonTargetKey << "' for non-targets" << endl;
		s << "Starting in " << i;

		Draw::centeredString(Display.getCenterOfDisplay(), s.str(), letterFont);
	}

	//Now load the first nBack + 1 stimuli into the slide presenter.
	for (unsigned int i = 0; i <= nBack; i++) {
		csp.beginDrawingNextSlide(stimulusPresentationDuration, "stimulus");
		drawStimulusForTrial(i, (i == nBack));
		csp.endDrawingCurrentSlide();

		csp.beginDrawingNextSlide(interStimulusInterval, "blank");
		ofBackground(backgroundColor);
		csp.endDrawingCurrentSlide();
	}
	trialNumber = nBack; //This will be the stimulus number that was just presented the first time the user function is called.

	//Once everything is set up, start presenting the slides.
	csp.startSlidePresentation();
}

void updateExperiment (void) {
	csp.update(); //Make sure that you call the update function of the csp, otherwise it does nothing.
}










void lastSlideFunction (CX_CSPInfo_t& info) {

	//At this point in time, the last slide has just been put on screen. The last slide is a blank, which means that the slide before it
	//was a stimulus that should have been responded to. We'll check for keyboard events.
	bool validResponseMade = false;
	if (Input.Keyboard.availableEvents() > 0) {
		//We don't want any responses made before the stimulus was presented, so let's find out when it was presented.
		CX_Slide_t &lastStimulusSlide = csp.getSlide( info.currentSlideIndex - 1 );
		CX_Micros_t stimulusOnset = lastStimulusSlide.actualSlideOnset;

		while (Input.Keyboard.availableEvents() > 0) {
			CX_KeyEvent_t kev = Input.Keyboard.getNextEvent();
			if ((kev.eventTime >= stimulusOnset) && (kev.eventType == CX_KeyEvent_t::PRESSED) && (kev.key == targetKey || kev.key == nonTargetKey)) {

				if (kev.key == targetKey) {
					df(trialNumber, "responseType") = "target";
				} else if (kev.key == nonTargetKey) {
					df(trialNumber, "responseType") = "nonTarget";
				}

				df(trialNumber, "responseLatency") = kev.eventTime - stimulusOnset;

				validResponseMade = true;
			}
		}
	}

	if (!validResponseMade) {
		df(trialNumber, "responseType") = "noValidResponse";
		df(trialNumber, "responseLatency") = 0;
	}

	//Draw the next letter and the following blank.
	info.instance->beginDrawingNextSlide(stimulusPresentationDuration, "stimulus");
	drawStimulusForTrial(trialNumber, true);

	info.instance->beginDrawingNextSlide(interStimulusInterval, "blank");
	ofBackground(backgroundColor);
	info.instance->endDrawingCurrentSlide();

	if (++trialNumber == trialCount) {
		info.userStatus = CX::CX_CSPInfo_t::STOP_NOW;
		df.printToFile("N-Back output.txt");

		Display.beginDrawingToBackBuffer();
		ofBackground(backgroundColor);
		Draw::centeredString(Display.getCenterOfDisplay(), "Experiment complete!", letterFont);

		Display.endDrawingToBackBuffer();
		Display.BLOCKING_swapFrontAndBackBuffers();

		ofSleepMillis(3000);
		ofExit();
	} else {
		info.userStatus = CX::CX_CSPInfo_t::CONTINUE_PRESENTATION;
	}

	Log.flush(); //For this experiment, this is probably the best time to flush the logs, but it is hard to say. You could simply wait until
		//the experiment is finished to flush.

}

void generateTrials(int numberOfTrials) {

	trialCount = numberOfTrials;

	string letterArray[8] = { "A", "F", "H", "L", "M", "P", "R", "Q" };
	vector<string> letters = arrayToVector(letterArray, 8); //Once c++11 is fully implemented, you will be able 
		//to use an initializer list for vectors as well as arrays. Until then, arrayToVector is useful.

	//Draw trialCount deviates from a binomial distribution with 1 trial and 40% probability of a success (i.e. trialCount slightly unfair coin flips).
	//For a real N-Back task, you would probably use a more complicated way of determining the trial sequence.
	vector<int> targetTrial = RNG.binomialDeviates(trialCount, 1, .4);

	//For the first N trials, pick letters randomly
	for (int i = 0; i < nBack; i++) {
		df(i, "letter") = RNG.sample(letters);
	}

	//From N on, pick based on trial type
	for (int i = nBack; i < trialCount; i++) {
		if (targetTrial[i] == 1) {
			df(i, "trialType") = "target";
			df(i, "letter") = df(i - nBack, "letter").toString();
		} else {
			df(i, "trialType") = "nonTarget";
			df(i, "letter") = RNG.randomExclusive(letters, df(i - nBack, "letter").toString());
		}
	}

	//Print out the current state of the data frame to make sure that everything looks normal.
	cout << df.print() << endl;
	cout << endl;
}

void drawStimulusForTrial(unsigned int trial, bool showInstructions) {
	string letter = df(trial, "letter").toString();

	ofBackground(backgroundColor);
	ofSetColor(textColor);
	Draw::centeredString(Display.getCenterOfDisplay(), letter, letterFont);

	if (showInstructions) {
		stringstream s;
		s << "Press '" << targetKey << "' for targets and '" << nonTargetKey << "' for non-targets";
		instructionFont.drawString(s.str(), 30, Display.getResolution().y - 30);
	}
}