#include "PlayerControls.h"

void PlayerControlsSystem::on_tick()
{
    const auto& keys = KeyState::get();

    for (auto&& [player_entity, player, bindings, pos, sprite, flip] : access_storage().each())
    {
        if (keys.is_pressed(bindings.up) && player.fsm.currentState != PlayerState::AIRBORNE) // keys.is_down() causes a bug where player can levitate trough platforms 
        {
            player.vertical_velocity = -JUMP_MOD;
            player.fsm.change_state(PlayerTransition::JUMP);
        }
        if (keys.is_pressed(bindings.down) && player.fsm.currentState != PlayerState::AIRBORNE) 
        {
            player.fsm.change_state(PlayerTransition::FALL);
            pos.xy.y += 5;
            if (pos.xy.y > SCREEN_HEIGHT - 16) 
            {
                pos.xy.y = SCREEN_HEIGHT - 16;
            }
        }
        if (keys.is_down(bindings.left))
        {
            player.fsm.change_state(PlayerTransition::WALK);
            replace_component<Flip>(player_entity, Horizontal);
            sprite.change_to("test/WizardRun");
            pos.xy.x -= SPEED_MOD * Time::delta_time();
            if (pos.xy.x < 11) 
            {
                pos.xy.x = 11;
            }
        }
        if (keys.is_down(bindings.right))
        {
            player.fsm.change_state(PlayerTransition::WALK);
            replace_component<Flip>(player_entity, None);
            sprite.change_to("test/WizardRun");
            pos.xy.x += SPEED_MOD * Time::delta_time();
            if (pos.xy.x > SCREEN_WIDTH - 11) 
            {
                pos.xy.x = SCREEN_WIDTH - 11;
            }
        }
        else if (!keys.is_down(bindings.left))
        {
            sprite.change_to("test/WizardIdle");
        }
        if (player.fsm.currentState == PlayerState::WALKING && (keys.is_released(bindings.left) || keys.is_released(bindings.right)))
        {
            player.fsm.change_state(PlayerTransition::STOP);
        }
        if (keys.is_released(bindings.up) && player.vertical_velocity < 0)
        {
            player.vertical_velocity/=2;
        }

        // Always happening vertical velocity movement
        pos.xy.y += player.vertical_velocity * Time::delta_time();
    }
}