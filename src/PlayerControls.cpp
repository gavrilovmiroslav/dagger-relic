#include "PlayerControls.h"

void PlayerControlsSystem::on_tick()
{
    const auto& keys = KeyState::get();

    for (auto&& [player_entity, player, bindings, pos, flip] : access_storage().each())
    {
        if (keys.is_pressed(bindings.up) && player.fsm.currentState != PlayerState::AIRBORNE) // keys.is_down() causes a bug where player can levitate trough platforms 
        {
            player.vertical_velocity = -JUMP_MOD;
            player.fsm.trigger(PlayerTransition::JUMP);
        }
        if (keys.is_pressed(bindings.down) && player.fsm.currentState != PlayerState::AIRBORNE) 
        {
            player.fsm.trigger(PlayerTransition::FALL);
            pos.xy.y += 5;
            if (pos.xy.y > SCREEN_HEIGHT - 16) 
            {
                pos.xy.y = SCREEN_HEIGHT - 16;
            }
        }
        if (keys.is_down(bindings.left) && !keys.is_down(bindings.right))
        {
            player.fsm.trigger(PlayerTransition::WALK);
            replace_component<Flip>(player_entity, Horizontal);
            pos.xy.x -= SPEED_MOD * Time::delta_time();
            if (pos.xy.x < 11) 
            {
                pos.xy.x = 11;
            }
        }
        if (keys.is_down(bindings.right) && !keys.is_down(bindings.left))
        {
            player.fsm.trigger(PlayerTransition::WALK);
            replace_component<Flip>(player_entity, None);
            pos.xy.x += SPEED_MOD * Time::delta_time();
            if (pos.xy.x > SCREEN_WIDTH - 11) 
            {
                pos.xy.x = SCREEN_WIDTH - 11;
            }
        }
        if ((player.fsm.currentState == PlayerState::WALKING) && !keys.is_down(bindings.left) && !keys.is_down(bindings.right))
        {
            player.fsm.trigger(PlayerTransition::STOP);
        }
        if (keys.is_released(bindings.up) && player.vertical_velocity < 0)
        {
            player.vertical_velocity/=2;
        }

        // Always happening vertical velocity movement
        pos.xy.y += player.vertical_velocity * Time::delta_time();
    }
}