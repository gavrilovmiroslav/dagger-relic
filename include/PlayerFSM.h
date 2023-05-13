#pragma once

#include "Prelude.h"
#include "FSM.h"

enum class PlayerState
{
    IDLE, 
    WALKING,
    ATTACKING

};

enum class PlayerTransition
{
    Walk,
    Attack,
    Stop
};

class PlayerReactor : public FSMReactor<PlayerState> 
{

    public:
        virtual void on_enter(PlayerState& s, SpriteAnimation &sprite) override
        {
            //some logic which needs to be done when entering state s (sprite changing etc.)
            switch(s)
            {
                case PlayerState::IDLE:
                {
                    //change sprite to the idle one
                    sprite.change_to("Moose/Moose1_Idle");
                    break;
                }
                case PlayerState::WALKING:
                {
                    sprite.change_to("Moose/Moose1_run");
                    break;
                }
                case PlayerState::ATTACKING:
                {
                    sprite.change_to("Moose/Moose1_attack_all_effects");
                    break;
                }
                default:
                    break;
            }

        }
        virtual void on_exit(PlayerState &s, SpriteAnimation &sprite) override
        {
            std::cout << "Exiting state "<< std::endl;
        }
};

class PlayerFSM : public FSM<PlayerState, PlayerTransition, PlayerReactor>,
                  public ecs::System
{ 
    
    private:
        entt::entity ent_id;
        PlayerReactor *r = new PlayerReactor();
    
    public:
        PlayerFSM()
        : FSM(r)
        {
            define_transitions(PlayerState::IDLE, PlayerTransition::Walk, PlayerState::WALKING);
            define_transitions(PlayerState::IDLE, PlayerTransition::Attack, PlayerState::ATTACKING);
            define_transitions(PlayerState::IDLE, PlayerTransition::Stop, PlayerState::IDLE);

            define_transitions(PlayerState::WALKING, PlayerTransition::Stop, PlayerState::IDLE);
            define_transitions(PlayerState::WALKING, PlayerTransition::Attack, PlayerState::ATTACKING);
            define_transitions(PlayerState::WALKING, PlayerTransition::Walk, PlayerState::WALKING);

            define_transitions(PlayerState::ATTACKING, PlayerTransition::Stop, PlayerState::IDLE);

            current_state = PlayerState::IDLE;
        }
};