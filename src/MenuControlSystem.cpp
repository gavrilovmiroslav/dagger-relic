#include "MenuControlSystem.h"

void MenuControlSystem::on_tick()
{
	const auto& key = KeyState::get();

	for (auto&& [entity, menu, key_binding] : access_storage().each())
	{
		if (key.is_pressed(key_binding.escape))
		{
			menu.m_fsm.trigger(MenuTransition::EXIT);
			Engine::get_instance().quit();
		}
		if (key.is_pressed(key_binding.a))
		{
			menu.m_fsm.trigger(MenuTransition::PAUSE);
		}
		if (key.is_pressed(key_binding.p))
		{
			menu.m_fsm.trigger(MenuTransition::PLAY);
		}
		if ((menu.m_fsm.currentState == MenuState::PLAYING) && key.is_pressed(key_binding.a) && !key.is_pressed(key_binding.p) && !key.is_pressed(key_binding.escape))
        	{
        		menu.m_fsm.trigger(MenuTransition::PAUSE);
        	}
		if ((menu.m_fsm.currentState == MenuState::PAUSING) && key.is_pressed(key_binding.p) && !key.is_pressed(key_binding.escape) && !key.is_pressed(key_binding.a))
        	{
        		menu.m_fsm.trigger(MenuTransition::PLAY);
        	}
		if ((menu.m_fsm.currentState == MenuState::PAUSING) && key.is_pressed(key_binding.escape) && !key.is_pressed(key_binding.p) && !key.is_pressed(key_binding.a))
        	{
        		menu.m_fsm.trigger(MenuTransition::EXIT);
			Engine::get_instance().quit();
        	}
	}
}
