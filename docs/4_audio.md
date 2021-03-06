Audio Input and Output {#audioIO}
======================

Audio input and output in CX is based on a number of classes. The two most important are \ref CX::CX_SoundStream "CX_SoundStream" and \ref CX::CX_SoundBuffer "CX_SoundBuffer". 
Additionally, \ref CX::CX_SoundBufferPlayer "CX_SoundBufferPlayer" and \ref CX::CX_SoundBufferRecorder "CX_SoundBufferRecorder" combine together a CX_SoundStream and a CX_SoundBuffer to play back or record audio, respectively.
Finally, for people wanting to synthesize audio in real time (or ahead of time), the CX::Synth namespace provides a multitude of ways to synthesize audio.
We will go through these components in a practical order.


Setting up the CX_SoundStream for Playback
------------------------------------------

Because the CX_SoundStream is what actually does audio input and output, in order to get any sounds out of a CX program, you must configure a CX_SoundStream for use. This requires that a \ref CX::CX_SoundStream::Configuration "CX_SoundStream::Configuration" struct be filled out with the desired settings and given to \ref CX::CX_SoundStream::setup() "CX_SoundStream::setup()" as the argument.
There are several configuration options for the CX_SoundStream, but, if the gods smile on you today, you will only need one, which is the number of output channels. If you try this and the gods seem to be frowning, check out the Troubleshooting Audio Problems section below.
We will use stereo output, so 2 output channels.

\code{.cpp}
CX_SoundStream::Configuration ssConfig;
ssConfig.outputChannels = 2; //Stereo output
//ssConfig.api = RtAudio::Api::WINDOWS_DS; //The most likely thing you will need to change is the low-level audio API.

//Create the CX_SoundStream and set it up with the configuration.
CX_SoundStream soundStream;
soundStream.setup(ssConfig);

//Check for any error messages.
Log.flush();
\endcode

If there were any errors during setup of the sound stream, they will be logged. Check the console for any messages. You can also check if the return value of `setup()` or call \ref CX::CX_SoundStream::isStreamRunning() "CX_SoundStream::isStreamRunning()" to see if setup was successful.

Playback
--------

Now that we have a CX_SoundStream set up, next next thing we need to do in order to play the contents of the sound file is to load the file
into CX. This is done by creating a CX_SoundBuffer and then loading a sound file into the sound buffer, as follows.

\code{.cpp}
CX_SoundBuffer soundBuffer;
soundBuffer.loadFile("sound_file.wav");
\endcode

If there wasn't an error loading the file, `soundBuffer` now contains the contents of the sound file in a format that can be played by CX. Once you have a sound file loaded into a CX_SoundBuffer, there are a number of things you can do with it. 
You can remove leading silence with CX_SoundBuffer::stripLeadingSilence() or add silence to the beginning or end with CX_SoundBuffer::addSilence().
You can delete part of the sound, starting from the beginning or end, with CX_SoundBuffer::deleteAmount().
You can reverse the order of the samples, so as to be able to play the sound backwards with CX_SoundBuffer::reverse().
These are just some examples. See the documentation for \ref CX::CX_SoundBuffer "CX_SoundBuffer" and the `soundBuffer` example for more things you can do with it.

Now that you have CX_SoundBuffer with sound data loaded into it, you can play it back using a \ref CX::CX_SoundBufferPlayer "CX_SoundBufferPlayer". Before you can use a CX_SoundBufferPlayer, you have to configure it with \ref CX::CX_SoundBufferPlayer::setup() "CX_SoundBufferPlayer::setup()". `setup()` takes either a structure holding configuration options for the CX_SoundStream that will be used by the CX_SoundBufferPlayer or a pointer to a CX_SoundStream that has already been set up. We will use the CX_SoundStream called `soundStream` that we configured in the previous section.

\code{.cpp}
CX_SoundBufferPlayer player;
player.setup(&soundStream);
\endcode

Now that we have a configured CX_SoundBufferPlayer, we just need to give it a CX_SoundBuffer to play by using \ref CX::CX_SoundBufferPlayer::setSoundBuffer() "CX_SoundBufferPlayer::setSoundBuffer()" and play the sound.

\code{.cpp}
player.setSoundBuffer(&soundBuffer);

player.play();

//Wait for it to finish playing.
while (player.isPlaying())
	;
\endcode

Because playback does not happen in the main thread, we wait in the main thread until playback is complete before going on.

Playing Multiple Sounds Simultaneously
--------------------------------------

A CX_SoundBufferPlayer can have a single CX_SoundBuffer assigned to it as the active sound buffer. This means that you cannot play more than one sound at once with a CX_SoundBufferPlayer. 
This limitation is by design, but also by design there are ways to play multiple sounds at once. The preferred way involves merging together multiple CX_SoundBuffers using \ref CX::CX_SoundBuffer::addSound() "CX_SoundBuffer::addSound()". What this does is take a CX_SoundBuffer and add it to an another CX_SoundBuffer at a given offset. This guarantees that the two sounds will be played at the correct time relative to one another, because there is no latency when the second sound starts playing. An example of merging sound buffers:

\code{.cpp}
CX_SoundBuffer otherBuffer;
otherBuffer.loadFile("other_sound_file.wav");

CX_SoundBuffer combinedBuffer = soundBuffer;

combinedBuffer.addSound(otherBuffer, 500); //Add the second sound to the first, 
	//with the second starting 500 ms after the first.
	
player.setSoundBuffer(&combinedBuffer);
player.play();
while(player.isPlaying())
	;
\endcode

Another way to play multiple sounds at once is to create multiple CX_SoundBufferPlayers, all of which use the same CX_SoundStream. Then you can assign different CX_SoundBuffers to each player and call CX_SoundBufferPlayer::play() whenever you want to play the specific sound.

\code{.cpp}
CX_SoundBufferPlayer player2;
player2.setup(&soundStream);
player2.setSoundBuffer(&otherBuffer);

player.play();
Clock.sleep(500);
player2.play();
while (player.isPlaying() || player2.isPlaying())
	;
\endcode

You can also put multiple CX_SoundBuffers and CX_SoundBufferPlayers into C++ standard library containers, like `std::vector`. However, I must again stress that using \ref CX::CX_SoundBuffer::addSound() "CX_SoundBuffer::addSound()" is a better way to do things because it provides 100% predictable relative onset times of sounds (unless there are glitches in audio playback, but that's a different serious problem).

Recording Audio
---------------

To record audio, you can use a \ref CX::CX_SoundBufferRecorder "CX_SoundBufferRecorder". You set it up with a CX_SoundStream, just like CX_SoundBufferPlayer. The only difference is that for recording, we need input channels instead of output channels. We will stop the currenly
running CX_SoundStream and reconfigure it to also have 1 input channel. We then set up the CX_SoundBufferRecorder using `soundStream`, create a new CX_SoundBuffer for it to record into, and set that buffer to be recorded to.

\code{.cpp}
soundStream.stop();
ssConfig.inputChannels = 1; //Most microphones are mono.
soundStream.setup(ssConfig);

CX_SoundBufferRecorder recorder;
recorder.setup(&soundStream);

CX_SoundBuffer recordedSound;
recorder.setSoundBuffer(&recordedSound);

Log.flush(); //As usual, let's check for errors during setup.
\endcode

Now that we have set up the recorder, we will record for 5 seconds, then play back what we have recorded.

\code{.cpp}
cout << "Starting to record." << endl;
recorder.start();
Clock.sleep(CX_Seconds(5));
recorder.stop();
cout << "Done recording." << endl;
\endcode

We sleep the main thread for 5 seconds while the recording takes place in a secondary thread. The implication of the use of secondary threads for recording is that you can start a recording, do whatever you feel like in the main thread -- draw visual stimuli, collect responses, etc. -- all while the recording keeps happening in a secondary thread.

Once our recording time is complete, we will set a CX_SoundBufferPlayer to play the recorded sound in the normal way.

\code{.cpp}
player.setSoundBuffer(&recordedSound);
player.play();
while (player.isPlaying())
	;
\endcode

Be careful that you are not recording to a sound buffer at the same time you are playing it back, because who knows what might happen (it would probably be fine, actually). To be careful, you can "detach" a CX_SoundBuffer from either a player or a recorder by calling, e.g., CX_SoundBufferPlayer::setSoundBuffer() with `nullptr` as the argument.

\code{.cpp}
recorder.setSoundBuffer(nullptr); //Make it so that no buffers are associated with the recorder.
\endcode


All of the pieces of code from above in one place:
\code{.cpp}
#include "CX.h"

void runExperiment(void) {
	//Sound stream configuration
	CX_SoundStream::Configuration ssConfig;
	ssConfig.outputChannels = 2; //Stereo output
	//ssConfig.api = RtAudio::Api::WINDOWS_DS; //The most likely thing you will need to change is the low-level audio API.

	//Create the CX_SoundStream and set it up with the configuration.
	CX_SoundStream soundStream;
	soundStream.setup(ssConfig);

	//Check for any error messages.
	Log.flush();

	//If things aren't working, try uncommenting this line to learn about the devices available on your system for the given api.
	//cout << CX_SoundStream::listDevices(RtAudio::Api::WINDOWS_DS) << endl;

	//Playback
	CX_SoundBuffer soundBuffer;
	soundBuffer.loadFile("sound_file.wav");

	CX_SoundBufferPlayer player;
	player.setup(&soundStream);
	player.setSoundBuffer(&soundBuffer);

	player.play();

	//Wait for it to finish playing.
	while (player.isPlaying())
		;

	Log.flush();

	soundBuffer.deleteChannel(1);
	player.setSoundBuffer(&soundBuffer);

	player.play();

	//Wait for it to finish playing.
	while (player.isPlaying())
		;

	Log.flush();


	//Playing multiple sounds at once
	CX_SoundBuffer otherBuffer;
	otherBuffer.loadFile("other_sound_file.wav");

	CX_SoundBuffer combinedBuffer = soundBuffer;

	combinedBuffer.addSound(otherBuffer, 500); //Add the second sound to the first, 
	//with the second starting 500 ms after the first.

	player.setSoundBuffer(&combinedBuffer);
	player.play();
	while (player.isPlaying())
		;

	CX_SoundBufferPlayer player2;
	player2.setup(&soundStream);
	player2.setSoundBuffer(&otherBuffer);

	player.play();
	Clock.sleep(500);
	player2.play();
	while (player.isPlaying() || player2.isPlaying())
		;


	//Recording
	soundStream.stop();
	ssConfig.inputChannels = 1; //Most microphones are mono.
	soundStream.setup(ssConfig);


	CX_SoundBufferRecorder recorder;
	recorder.setup(&soundStream);

	CX_SoundBuffer recordedSound;
	recorder.setSoundBuffer(&recordedSound);

	Log.flush(); //As usual, let's check for errors during setup.

	cout << "Starting to record." << endl;
	recorder.start();
	Clock.sleep(CX_Seconds(5));
	recorder.stop();
	cout << "Done recording." << endl;

	player.setSoundBuffer(&recordedSound);
	player.play();
	while (player.isPlaying())
		;

	recorder.setSoundBuffer(nullptr); //Make it so that no buffers are associated with the recorder.
}
\endcode


Synthesizing Audio
------------------

You can synthesize audio in real time, or ahead of time, using the classes in the CX::Synth namespace. See the modularSynth example for some uses of synthesizer modules.


Direct Control of Audio IO
--------------------------

If you want to be really fancy, you can directly read and write the audio data that a CX_SoundStream is sending or receiving. This is a relatively advanced operation and is unlikely to be needed in very many cases, but it's there if need be.

In order to directly access the data that a CX_SoundStream is transmitting, you need to create a class containing a function that will be called every time the CX_SoundStream needs to send more data to the sound card. For example, you could have a class like this that creates a sine wave.

\code{.cpp}
class ExampleOutputClass {
public:
	void callbackFunction(CX_SoundStream::OutputEventArgs& args) {
		static float wavePosition = 0;

		float sampleRate = args.instance->getConfiguration().sampleRate;
		const float frequency = 524;
		float positionChangePerSampleFrame = 2 * PI * frequency / sampleRate;

		for (unsigned int sampleFrame = 0; sampleFrame < args.bufferSize; sampleFrame++) {

			//For every channel, put the same data on that channel. This is like playing a mono stream on every channel at the same time.
			for (unsigned int channel = 0; channel < args.outputChannels; channel++) {
				args.outputBuffer[(sampleFrame * args.outputChannels) + channel] = sin(wavePosition);
			}

			//Update where in the sine wave we are. A single sine wave happens every 2 * PI.
			wavePosition = fmod(wavePosition + positionChangePerSampleFrame, 2 * PI);
		}
	}
};
\endcode

The only thing going on in this class is `callbackFunction`. This function takes a reference to a CX_SoundStream::OutputEventArgs struct, which contains important data. Most importantly, `args`, as I have called it in this example, contains a pointer to an array of data that should be filled by the function, called `outputBuffer`. The number of sample frames of data that should be put into `outputBuffer` is given by `bufferSize`. It is important here to be clear about the fact that a sample frame contains 1 sample per channel of sound data, so if `bufferSize` is 256 and the stream is running in stereo (2 channels), the total number of samples that need to be put into `outputBuffer` must be 512. Also note that the sound samples must be interleaved, which means that samples within a sample frame are stored contiguously in the buffer, which means that for the stereo example, even numbered indices would contain data for channel 0 and off numbered indices would contain data for channel 1.

Of course, if you really wanted to create sine waves in real time, you would use CX::Synth::Oscillator and CX::Synth::StreamOutput, but for the sake of example, lets use this class.
Once you have defined a class that creates the sound data, create an instance of your class and add it as a listener to the `outputEvent` of a CX_SoundStream.

\code{.cpp}
	CX_SoundStream soundStream; //Assume this has been or will be set up elsewhere.
	ExampleOutputClass sineOut; //Make an instance of the class.
	
	//For event soundStream.outputEvent, targeting class instance sineOut, call callbackFunction of that class instance.
	ofAddListener(soundStream.outputEvent, &sineOut, &ExampleOutputClass::callbackFunction);
\endcode

From now on, whenever `soundStream` needs more output data, `sineOut.callbackFunction` will be called automatically. The data that you put into the output buffer must be of type `float` and bounded between -1 and 1, inclusive. You can remove a listener to an event with `ofRemoveListener`. More information about the events used by openFrameworks can be found here: http://www.openframeworks.cc/documentation/events/ofEvent.html.

Directly accessing input data works in a very similar way. You need a class with a function that takes a reference to a \ref CX::CX_SoundStream::InputEventArgs struct and returns `void`. Instead of putting data into the output buffer, you would read data out of the input buffer.


Troubleshooting Audio Problems 
------------------------------

It is often the case that audio playback problems arise due to the wrong input or output device being used. For this reason, CX_SoundStream has a utility function that lists the available devices on your system so that you can select the correct one. You do this with CX::CX_SoundStream::listDevices() like so:
\code{.cpp}
cout << CX_SoundStream::listDevices() << endl;
\endcode
Note that `listDevices()` is a static function, so you use the name of the CX_SoundStream class and `::` to access it.

CX_SoundStream uses RtAudio (http://www.music.mcgill.ca/~gary/rtaudio/) internally. It is possible that some problems could be solved with help from the RtAudio documentation. For example, one of the configuration options for CX_SoundStream is the low level audio API to use (see \ref CX::CX_SoundStream::Configuration::api), about which the RtAudio documentation provides some help (http://www.music.mcgill.ca/~gary/rtaudio/classRtAudio.html#ac9b6f625da88249d08a8409a9db0d849). You can get a pointer to the RtAudio instance being used by the CX_SoundStream by calling CX::CX_SoundStream::getRtAudioInstance(), which should allow you to do just about anything with RtAudio.


Audio Latency
-------------

OpenFrameworks and, by extension, CX use the RtAudio library, which provides a cross-platform wrapper for different audio APIs. RtAudio provides a consistent interface across platforms and APIs and that interface is what will be described here.

The core of how audio data is given to the hardware that creates the physical stimuls is very similar to the way in which visual stimuli are presented. For visual stimuli, there are two data buffers. One of the buffers is actively being presented while the other buffer is being filled with data in preparation for presentation. Similarly, for audio data, there are at least two buffers. One buffer is presented while another buffer is being filled with data. Whenever the sound hardware finishes presenting one buffer's worth of data, it starts presenting the next buffer of data and it requests a new buffer of data from the audio data source (e.g. a CX_SoundBufferPlayer). When the hardware is done presenting a buffer of data, if the next buffer is not ready, an audio glitch will occur. Typically what happens is that there is a brief moment of silence before the next buffer of data is ready. Also, "clicks" or "pops" are often heard surrounding the silence. Due to these glitches, it is very important that the next buffer of audio data can consistently be filled before the active buffer is emptied. Glitches can be reduced or eliminated by 1) using multiple buffers (which is possible with some audio APIs, but not many) or 2) increasing the size of the buffers. By increasing the size of the buffers, it increases the probability that a buffer of new data will be filled before the active buffer is emptied.

On the other hand, we would like to minimize latency. Latency occurs in audio data because at the time at which you request a sound to be played, there is already some amount of data already in the buffers ahead of the sound you want to play. The new sound that you requested is put at the back of the queue. Typically, the next time that a new buffer worth of data is requested, your sound data will be put into the start of that buffer. The fewer buffers that are in front of the new data, the sooner the new data will make its way to the front of the line and actually get presented. The smaller the buffers are, the smaller the delays between buffers, so your data can be queued more quickly. The best way to minimize latency is to 1) have fewer buffers (minimum of 2: the active buffer and the buffer being filled) and 2) reduce the size of the buffers. Clearly, there is a tradeoff between latency and glitches. The standard recommendation is to aggressively minimize latency until you start to notice glitches, then back off until you stop detecting glitches.

Notice that the latency as determined by the buffer size and number of buffers, while typically represented as a constant value, is not actually a constant. At the time at which the playback of a sound is requested, it could be that the buffers have just swapped, so the new sound will need to wait nearly the whole length of time that it takes to present a buffer before it even gets put into the queue. Another possibility is that the sound playback is requested right before a buffer swap, so the new data will be put into the queue right away. Thus, the typical latency measurement is more of an upper bound than a constant. CX_SoundBufferPlayer has functionality to help deal with this fact, in that you can request that sounds start at a specified time in the future using CX_SoundBufferPlayer::startPlayingAt(). When this function is used, when the sound data is to be put into a buffer, it is not necessarily put at the beginning of the buffer, but is put wherever in the buffer it needs to be in order to be played at the right time relative to the buffer onset. However, note that there may still be some constant latency involved even when this function is used that it does not account for.

CX_SoundStream has some functions like CX_Display for learning about the buffer swapping process. There are CX_SoundStream::hasSwappedSinceLastCheck(), CX_SoundStream::waitForBufferSwap(), CX_SoundStream::getLastSwapTime(), and CX_SoundStream::estimateNextSwapTime().

CX tries to use audio in as latency-controlled a way as possible. The way it does this is by opening an audio stream and keeping it open throughout an experiment. This helps to avoid the latency associated with creating a new audio stream every time a sound is played. This makes it so that the only latency comes from the number and size of the buffers that are used. By keeping an audio stream open all the time, there is a small CPU cost, but it is fairly small. In spite of the design of CX audio, there will always be latency, so understanding the reasons for the latency can help you to deal with it appropriately. 

One downside of the use of only a single stream of audio data is that if multiple sounds are to be played at once, the sound levels need to be adjusted in software in CX to prevent clipping. To explain why this is, a little background is needed. Sound data in CX is treated as a `float` and the amplitudes of the data go from -1 to 1. If an amplitude goes outside of this range, it will be clipped, which produces a type of distortion because the waveform is deformed by flattening the peaks of the waves. When multiple sounds are played at once in a single stream, their amplitudes are added, which means that even if each sound in within an acceptable range, once added together, they will be outside of the interval [-1,1] and clipping will occur. For this reason, when multiple sounds are to played together, regardless of whether more than one CX_SoundBufferPlayer is used or the sounds are merged together in a single CX_SoundBuffer, the ampltudes of the sounds need to be constrained. The easiest way of doing this is to use CX_SoundBuffer::normalize() on the sounds. Normalizing a sound means to make the highest peak in the sound have some set amplitude. Assume you had two sounds with unknown maximum amplitudes. If they were both normalized to have a maximum amplitude of 0.5 each, then when added together, they could never clip.


