#pragma once

#include "Definitions.h"

template<typename Transition, typename State>
class FSM
{
public:
    virtual void on_enter(State) = 0;
    virtual void on_exit(State) = 0;

    State currentState;

    void trigger(Transition transition)
    {
        if (validTransitionsFromState[currentState].find(transition) == validTransitionsFromState[currentState].end())
            return;

        on_exit(currentState);
        currentState = newState[transition];
        on_enter(currentState);
    }

    void add_transition_to_new_state(Transition t, State s)
    {
        newState.emplace(t, s);
    }

    void add_valid_transitions_from_state(State s, containers::Set<Transition> ts)
    {
        validTransitionsFromState.emplace(s, ts);
    }

private:
    // if a pair (t, s) is in the map, performing the transition t from the current state leads to state s
    containers::Map<Transition, State> newState;

    // for every state the map contains a set of valid transitions from that state
    containers::Map<State, containers::Set<Transition>> validTransitionsFromState;
};
