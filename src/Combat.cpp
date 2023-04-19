#include "Combat.h"
#include <iostream>

void ProjectileSpawnSystem::on_tick()
{
    const auto& keys = KeyState::get();

    for (auto&& [player_entity, player, bindings, pos, sprite, flip, item] : access_storage().each())
    {
        int direction = (flip == None) ? 1 : -1;
        if (keys.is_pressed(bindings.basic_attack)) 
        {
            spawn()
            .with<Spell>(300.0f, pos.xy.x + direction * 30.0f, 400.0f, 25.0f, 5)
            .with<Sprite>(ecs::no_entity)
            .with<SpriteAnimation>(Spritesheet::get_by_name("test/Fireball"))
            .with<Position>(geometry::Vec2{ pos.xy.x + direction * 30.0f , pos.xy.y })
            .with<Visibility>(true)
            .with<Flip>(flip)
            .with<Scale>(geometry::Vec2{0.1f,0.1f})
            .with<Element>(Fire)
            .with<Shape>(Ray)
            .with<Move>(Line)
            .with<Duration>(Disappear)
            .with<Clipping>(Pass);
        }

        else if (keys.is_pressed(bindings.special_attack) && item.uses > 0)
        {
            item.uses -= 1;
            if(item.name == "curseball")
            {
                spawn()
                .with<Spell>(100.0f, pos.xy.x + direction * 30.0f, 800.0f, 25.0f, 30)
                .with<Sprite>(ecs::no_entity)
                .with<SpriteAnimation>(Spritesheet::get_by_name("test/Curseball"))
                .with<Position>(geometry::Vec2{ pos.xy.x + direction * 30.0f , pos.xy.y })
                .with<Visibility>(true)
                .with<Flip>(flip)
                .with<Scale>(geometry::Vec2{0.1f,0.1f})
                .with<Element>(Fire)
                .with<Shape>(Ray)
                .with<Move>(Line)
                .with<Duration>(Disappear)
                .with<Clipping>(Pass);
            }
        }
    }
}

void SpellMovementSystem::on_tick()
{
    for (auto&& [spell_entity, spell, spell_pos, sprite, flip, move, clipping] : QuerySpells::access_storage().each())
    {
        if(move == Line)
        {
            int direction = (flip == None) ? 1 : -1;
            spell_pos.xy.x += direction * spell.speed * Time::delta_time();
            if(abs(spell_pos.xy.x - spell.start_pos) > spell.range){
                despawn(spell_entity);
            }
        }

        for (auto&& [player_entity, player, player_pos] : QueryPlayers::access_storage().each())
        {
            if (abs(player_pos.xy.x - spell_pos.xy.x) <= spell.radius &&
             	abs(player_pos.xy.y - spell_pos.xy.y) <= spell.radius)
            {
                PlayerCollisionEmitter::emit(SpellPlayerCollisionSignal{player_entity, spell_entity});
            }
        }

        if(clipping == Collides) // Only if the spell is supposed to collide with platforms
        {
            for (auto&& [platform_entity, platform, platform_pos] : QueryPlatforms::access_storage().each())
            {
                if (spell_pos.xy.x <= platform_pos.xy.x + platform.width / 2 + spell.radius &&
                    spell_pos.xy.x >= platform_pos.xy.x - platform.width / 2 - spell.radius && 
                    spell_pos.xy.y + spell.radius >= platform_pos.xy.y - 1 && 
                    spell_pos.xy.y + spell.radius <= platform_pos.xy.y + 1) 
                {
                    PlatformCollisionEmitter::emit(SpellPlatformCollisionSignal{platform_entity, spell_entity});
                }
            }
        }
    }
}

void SpellCollisionSystem::process_signal(SpellPlayerCollisionSignal& signal){
    if (is_ok(signal.player) && is_ok(signal.spell))
    {
        if (auto* spell = MutAccessComponentById<Spell>::get(signal.spell))
		{
			if (auto* status = MutAccessComponentById<Status>::get(signal.player))
            {
                status->health -= spell->damage;
                if(status->health <= 0) 
                {
                    despawn(signal.player);
                }
            }
		}
    }
    if (is_ok(signal.spell))
    {
        if (auto* linger = MutAccessComponentById<Duration>::get(signal.spell))
        {
            if (*linger != Lingers){
                despawn(signal.spell);
            }
        }
    }
}