#include "Combat.h"

void ProjectileSpawnSystem::on_tick()
{
    const auto& keys = KeyState::get();

    for (auto&& [player_entity, player, bindings, pos, sprite, flip, item] : access_storage().each())
    {
        if (keys.is_pressed(bindings.basic_attack))
        {
            if (flip == None)
            {
                auto attack = spawn()
                .with<Spell>(300.0f, pos.xy.x + 400.0f, 25.0f)
                .with<Sprite>(ecs::no_entity)
                .with<SpriteAnimation>(Spritesheet::get_by_name("test/Fireball"))
                .with<Position>(geometry::Vec2{ pos.xy.x + 30.0f , pos.xy.y })
                .with<Visibility>(true)
                .with<Flip>(None)
                .with<Scale>(geometry::Vec2{0.1f,0.1f})
                .with<Element>(Fire)
                .with<Shape>(Ray)
                .with<Move>(Line)
                .with<Duration>(Disappear)
                .with<Clipping>(Pass);
            }
            else
            {
                auto attack = spawn()
                .with<Spell>(300.0f, pos.xy.x - 400.0f, 25.0f)
                .with<Sprite>(ecs::no_entity)
                .with<SpriteAnimation>(Spritesheet::get_by_name("test/Fireball"))
                .with<Position>(geometry::Vec2{ pos.xy.x - 30.0f , pos.xy.y })
                .with<Visibility>(true)
                .with<Flip>(Horizontal)
                .with<Scale>(geometry::Vec2{0.1f,0.1f})
                .with<Element>(Fire)
                .with<Shape>(Ray)
                .with<Move>(Line)
                .with<Duration>(Disappear)
                .with<Clipping>(Pass);
            }
        }

        else if (keys.is_pressed(bindings.special_attack))
        {
            if(item.name == "curseball")
            {
                if (flip == None)
                {
                    auto attack = spawn()
                    .with<Spell>(300.0f, pos.xy.x + 400.0f, 25.0f)
                    .with<Sprite>(ecs::no_entity)
                    .with<SpriteAnimation>(Spritesheet::get_by_name("test/Curseball"))
                    .with<Position>(geometry::Vec2{ pos.xy.x + 30.0f , pos.xy.y })
                    .with<Visibility>(true)
                    .with<Flip>(None)
                    .with<Scale>(geometry::Vec2{0.1f,0.1f})
                    .with<Element>(Fire)
                    .with<Shape>(Ray)
                    .with<Move>(Line)
                    .with<Duration>(Disappear)
                    .with<Clipping>(Pass);
                }
                else
                {
                    auto attack = spawn()
                    .with<Spell>(300.0f, pos.xy.x - 400.0f, 25.0f)
                    .with<Sprite>(ecs::no_entity)
                    .with<SpriteAnimation>(Spritesheet::get_by_name("test/Curseball"))
                    .with<Position>(geometry::Vec2{ pos.xy.x - 30.0f , pos.xy.y })
                    .with<Visibility>(true)
                    .with<Flip>(Horizontal)
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
}

void SpellMovementSystem::on_tick()
{
    for (auto&& [spell_entity, spell, spell_pos, sprite, flip, move, clipping] : QuerySpells::access_storage().each())
    {
        if(move == Line)
        {
            if(flip == None)
            {
                spell_pos.xy.x += spell.speed * Time::delta_time();
                if(spell_pos.xy.x > spell.range)
                {
                    despawn(spell_entity);
                }
            }
            else
            {
                spell_pos.xy.x -= spell.speed * Time::delta_time();
                if(spell_pos.xy.x < spell.range)
                {
                    despawn(spell_entity);
                }
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
    despawn(signal.player);
    auto &linger = MutAccessComponentById<Duration>::get(signal.spell);
    if (linger != Lingers){
        despawn(signal.spell);
    }
}