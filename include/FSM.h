#pragma once
#include "Access.h"
#include "Definitions.h"

template<typename State>
struct FSMReactor
{
    virtual void on_enter(State& s) = 0;
    virtual void on_exit(State& s) = 0;
};

template<typename State, typename Transition, typename Reactor>
struct FSM
{
    State current_state;
    containers::Map<State, containers::Map<Transition, State>> transitions;

    Reactor* reactor;

    FSM(Reactor* r) : reactor{r} 
    {}
    
    void define_transitions(State from, Transition tr, State to)
    {
        if (transitions.count(from) == 0)
        {
            transitions[from] = containers::Map<Transition, State>();
        }

        transitions[from][tr] = to;
    }

    Bool trigger(Transition tr)
    {
        if (transitions[current_state].count(tr) == 0)
        {
            return false;
        }

        reactor->on_exit(current_state);
        current_state = transitions[current_state][tr];
        reactor->on_enter(current_state);
        return true;
    }
};
