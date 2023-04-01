#include "PlayerControls.h"

void PlayerControlsSystem::on_tick()
{
    const auto& keys = KeyState::get();

    for (auto&& [player_entity, player, bindings, pos, sprite, flip] : access_storage().each())
    {
        if (keys.is_pressed(bindings.up) && player.is_grounded) // keys.is_down() causes a bug where player can levitate trough platforms 
        {
            player.jump_speed = JUMP_MOD;
        }
        if (keys.is_down(bindings.down))  // deleted player.is_grounded condition, player can now go down from platform
        {
            pos.xy.y += SPEED_MOD * Time::delta_time();
            if (pos.xy.y > SCREEN_HEIGHT - 16) 
            {
                pos.xy.y = SCREEN_HEIGHT - 16;
            }
        }
        if (keys.is_down(bindings.left))
        {
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
        if (player.jump_speed > 0) 
        {
            pos.xy.y -= player.jump_speed * Time::delta_time();
            player.jump_speed -= 0.1;
        }
        if (keys.is_released(bindings.up))
        {
            player.is_grounded = false;
            player.jump_speed/=2;
        }
    }
}