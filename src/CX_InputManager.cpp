#include "CX_InputManager.h"

#include "CX_AppWindow.h" //glfwPollEvents()
#include "CX_Private.h"

namespace CX {

	namespace Private {
		CX_InputManager inputManagerFactory(void) {
			return CX_InputManager();
		}
	}

	CX::CX_InputManager CX::Instances::Input = CX::Private::inputManagerFactory();

	CX_InputManager::CX_InputManager(void) :
		Keyboard(this),
		Mouse(this),
		_usingJoystick(false)
	{
	}

	/*!
	Set up the input manager to use the requested devices. You may call this function multiple times if you want to
	change the configuration over the course of the experiment. Every time this function is called, all input device
	events are cleared.
	\param useKeyboard Enable or disable the keyboard.
	\param useMouse Enable or disable the mouse.
	\param joystickIndex Optional. If >= 0, an attempt will be made to set up the joystick at that index. If < 0, no attempt will
	be made to set up the joystick.
	\return `false` if the requested joystick could not be set up correctly, `true` otherwise.
	*/
	bool CX_InputManager::setup(bool useKeyboard, bool useMouse, int joystickIndex) {

		pollEvents(); //Flush out all waiting events during setup.

		Keyboard.clearEvents();
		Keyboard.enable(useKeyboard);

		Mouse.clearEvents();
		Mouse.enable(useMouse);

		bool success = true;
		if (joystickIndex >= 0) {
			Joystick.clearEvents();
			success = success && Joystick.setup(joystickIndex);
			if (success) {
				_usingJoystick = true;
			}
		}
		return success;
	}

	/*!	This function polls for new events on all of the configured input devices (see CX_InputManager::setup()). 
	After a call to this function, new events for the input devices can be found by checking the availableEvents() 
	function for each device.
	\return `true` if there are any events available for enabled devices, `false` otherwise. Note that the events 
	do not neccessarily	need to be new events in order for this to return `true`. If there were events that were 
	already stored in Mouse, Keyboard, or Joystick that had not been processed by user code at the time this 
	function was called, this function will return `true`. */
	bool CX_InputManager::pollEvents(void) {
#ifdef TARGET_RASPBERRY_PI
		((CX::Private::CX_RPiAppWindow*)CX::Private::appWindow.get())->checkEvents();
#else
		glfwPollEvents();
#endif
		CX_Millis pollCompleteTime = CX::Instances::Clock.now();

		if (_usingJoystick) {
			Joystick.pollEvents();
		}

		if (Mouse.enabled()) {
			Mouse._lastEventPollTime = pollCompleteTime;
		} else {
			Mouse.clearEvents();
		}

		if (Keyboard.enabled()) {
			Keyboard._lastEventPollTime = pollCompleteTime;
		} else {
			Keyboard.clearEvents();
		}

		if (Mouse.availableEvents() > 0 || Keyboard.availableEvents() > 0 || Joystick.availableEvents() > 0) {
			return true;
		}
		return false;
	}

	/*! This function clears all events on all input devices.
	\param poll If `true`, events are polled before they are cleared, so that events that hadn't yet
	made it into the device specific queues (e.g. the Keyboard queue) are cleared as well. */
	void CX_InputManager::clearAllEvents(bool poll) {
		if (poll) {
			pollEvents();
		}
		Keyboard.clearEvents();
		Mouse.clearEvents();
		Joystick.clearEvents();
	}

}
