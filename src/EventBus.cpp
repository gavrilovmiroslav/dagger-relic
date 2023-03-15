#include "EventBus.h"
#include "Engine.h"
#include "Tooling.h"

using namespace core;

bool EventBusModule::on_start()
{
	// TODO: add joystick recognition and support (see https://www.libsdl.org/release/SDL-1.2.15/docs/html/guideinput.html)
	// TODO: figure out how to enable adding and removing joysticks in runtime after start
	return true;
}

void EventBusModule::on_tick()
{
	bus::Event e;
	static BusEventSignal signal;

	auto& key_state = MutAccessUnique<KeyState>::access_unique();

	key_state.pressed_keys.clear();
	key_state.released_keys.clear();

	bool quit = false;

	static int n = 0;

	while (bus::PollEvent(&e) != 0)
	{
		signal.event = e;

		switch (e.type)
		{
		case SDL_QUIT:
			quit = true;
			break;

		case SDL_WINDOWEVENT:
			if (e.window.event == SDL_WINDOWEVENT_RESIZED) {
				SignalEmitter<core::WindowResizedSignal>::emit(core::WindowResizedSignal{ (U32)e.window.data1, (U32)e.window.data2 });
			}
			break;

		case SDL_KEYDOWN:
		{
			auto code = (KeyCode)e.key.keysym.scancode;
			if (!key_state.current_keys.contains(code))
			{
				key_state.pressed_keys.add(code);
				key_state.current_keys.add(code);
			}
		}
			break;

		case SDL_KEYUP:
		{
			auto code = (KeyCode)e.key.keysym.scancode;
			if (key_state.current_keys.contains(code))
			{
				key_state.released_keys.add((KeyCode)e.key.keysym.scancode);
				key_state.current_keys.remove((KeyCode)e.key.keysym.scancode);
			}
		}
			break;

		default:
			break;
		}

		SignalEmitter<BusEventSignal>::emit(signal);

		if (quit)
			Engine::get_instance().quit();
	}

	auto& mouse_state = MutAccessUnique<MouseState>::access_unique();
	mouse_state.mouse_button_mask = SDL_GetMouseState(&mouse_state.mouse_position.x, &mouse_state.mouse_position.y);

	for (U8 i = 1; i < 4; i++)
	{
		U8 j = i - 1;
		mouse_state.current_mouse_buttons[j] = mouse_state.mouse_button_mask & SDL_BUTTON(i);

		if (!mouse_state.previous_mouse_buttons[j] && !mouse_state.current_mouse_buttons[j])
		{
			mouse_state.mouse_buttons[j] = MouseEventState::UpHold;
		}
		else if (!mouse_state.previous_mouse_buttons[j] && mouse_state.current_mouse_buttons[j])
		{
			mouse_state.mouse_buttons[j] = MouseEventState::DownNow;
		}
		else if (mouse_state.previous_mouse_buttons[j] && mouse_state.current_mouse_buttons[j])
		{
			mouse_state.mouse_buttons[j] = MouseEventState::DownHold;
		}
		else if (mouse_state.previous_mouse_buttons[j] && !mouse_state.current_mouse_buttons[j])
		{
			mouse_state.mouse_buttons[j] = MouseEventState::UpNow;
		}

		mouse_state.previous_mouse_buttons[j] = mouse_state.current_mouse_buttons[j];
	}
}