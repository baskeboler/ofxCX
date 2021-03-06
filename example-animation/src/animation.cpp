#include "CX.h"

/*
It is not immediately obvious how to do animations using CX while not blocking, 
so I've made an example showing how to do so.

There are really just four critical functions:
CX_Display::setAutomaticSwapping(), CX_Display::hasSwappedSinceLastCheck(),
CX_Display::beginDrawingToBackBuffer(), and CX_Display::endDrawingToBackBuffer().

All you have to do to set up the animation is to call:
CX_Display::setAutomaticSwapping(true);
This causes the contents of the back buffer to be automatically swapped to
the front buffer every monitor refresh.

Then, in updateAnimation(), check hasSwappedSinceLastCheck() to see if
a swap has just occurred. If so, use beginDrawingToBackBuffer() and 
endDrawingToBackBuffer() to draw whatever the next frame of the animation
is into the back buffer.

That's it!
*/

//These variables have to do with the contents of the animation
int mouseX = 100;
double circleRadius = 30;
double angles[3] = { 0, 0, 0 };
double angleMultiplier[3] = { 1, 2, 3 };
int directions[3] = { 1, 1, 1 };
double distancesFromCenter[3] = { 75, 150, 225 };
double distanceMultiplier = 1;
CX_Millis lastAnimationDrawTime = 0;

void updateAnimation (void);
void drawNextFrameOfAnimation (void);
ofPoint getCircleLocation(int circleIndex);

void runExperiment (void) {
	//Use mouse, but not keyboard.
	Input.setup(false, true);

	//The window needs to be about this size in order to fit the circles.
	Disp.setWindowResolution(600, 600);

	//See the main comment at the top of this file.
	Disp.setAutomaticSwapping(true);

	while (true) {
		updateAnimation();
	}
}

void updateAnimation (void) {

	//See the main comment at the top of this file.
	if (Disp.hasSwappedSinceLastCheck()) {

		Disp.beginDrawingToBackBuffer(); //Prepare to draw the next frame of the animation.

		drawNextFrameOfAnimation();

		Disp.endDrawingToBackBuffer(); //Make sure to call this to end the drawing.

		//Because the front and back buffers are automatically swapping, you don't
		//need to do anything else here: the new frame will be swapped to the front
		//at some point in the near future.
	}

	//Do a little bit of stuff to get the state of the mouse.
	Input.pollEvents();
	while (Input.Mouse.availableEvents() > 0) {
		CX_Mouse::Event mev = Input.Mouse.getNextEvent();

		switch (mev.type) {
		case CX_Mouse::MOVED:
			mouseX = mev.x;
			break;
		case CX_Mouse::PRESSED:
			//Check to see if a circle was clicked on
			for (int i = 0; i < 3; i++) {
				if (getCircleLocation(i).distance(ofPoint(mev.x, mev.y)) <= circleRadius) {
					directions[i] *= -1;
				}
			}
			break;
		case CX_Mouse::SCROLLED:
			//If the mouse was scrolled, change the distance of the circles from the center.
			distanceMultiplier += mev.y * .02; //The y component of the scroll wheel is the typical scroll wheel on most mice
			distanceMultiplier = Util::clamp(distanceMultiplier, -1.5, 1.5);
			break;
		}
	}
}

void drawNextFrameOfAnimation (void) {

	static ofColor colors[3] = { ofColor::red, ofColor::green, ofColor::blue };

	ofBackground(0);

	ofSetColor(255);
	ofDrawBitmapString("Move the mouse to the left or right to change speed.\n"
					   "Click on a circle to change its direction.\n"
					   "Use the mouse wheel to change the orbit size.", ofPoint(30, 30));

	//it's a good idea to have animations change as a function of time, not as a function of frames,
	//because if the animation is based on frames and a frame is missed the animation appears uneven.
	CX_Millis now = Clock.now();
	CX_Millis elapsedTime = now - lastAnimationDrawTime;
	lastAnimationDrawTime = now;

	for (int i = 0; i < 3; i++) {
		angles[i] += elapsedTime.seconds()/5 * mouseX * directions[i] * angleMultiplier[i];
		ofSetColor(colors[i]);
		ofCircle(getCircleLocation(i), circleRadius);
	}

}

ofPoint getCircleLocation(int circleIndex) {
	return Util::getRelativePointFromDistanceAndAngle(Disp.getCenter(), 
													  distancesFromCenter[circleIndex] * distanceMultiplier, 
													  angles[circleIndex]);
}