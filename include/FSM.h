#pragma once

#include "Access.h"
#include "Definitions.h"

#include <iostream>
#include <fstream>
#include <string>
#include <regex>

using State = String;
using Action = String;
using Transition = String;

struct FSMReaction 
{
    State from;
    Transition transition;
    Action action;
    State to;
};

enum class NodeType { None, State, Link };

struct FSM
{
private:
    containers::Map<State, containers::Map<Transition, State>> transitions;
    containers::Map<State, containers::Map<Transition, Action>> transition_with_actions;    
    containers::Map<Action, ecs::Dispatcher> action_dispatch;
    ecs::Dispatcher dispatcher;

public:
    State current_state;
    
    void define_state(State state) 
    {
        transitions[state] = containers::Map<Transition, State>();
        transition_with_actions[state] = containers::Map<Transition, Action>();
        Logger::info("Added state {}", state);
    }

    void define_transitions(State from, Transition tr, Action act, State to)
    {
        transitions[from][tr] = to;
        transition_with_actions[from][tr] = act;
        Logger::info("Added transition {} ---{}/{}--> {}", from, tr, act, to);
    }

    void define_action(Action action)
    {
        if (action_dispatch.count(action) == 0)
            action_dispatch[action] = ecs::Dispatcher{};
    }

    Bool is_action_defined(String key) const
    {
        return action_dispatch.count(key) > 0;
    }

    auto fsm_events()
    {
        return dispatcher.sink<FSMReaction>();
    }

    auto events_for_action(String key)
    {
        return action_dispatch[key].sink<FSMReaction>();
    }

    Bool trigger(Transition transition)
    {
        if (transitions[current_state].count(transition) == 0)
        {
            return false;
        }

        auto& next_state = transitions[current_state][transition];
        auto& action = transition_with_actions[current_state][transition];
        FSMReaction reaction{ current_state, transition, action, next_state };

        dispatcher.trigger(reaction);
        if (is_action_defined(action))
        {
            action_dispatch[action].trigger(reaction);
        }

        current_state = next_state;
        return true;
    }

    containers::DynamicArray<String> match(String input, String regex_entry)
    {    
        std::regex regex(regex_entry);
        std::smatch match;
        std::string::const_iterator search_start(input.cbegin());
        containers::DynamicArray<String> result;
        if (std::regex_search(search_start, input.cend(), match, regex)) 
        {
            for (I32 i = 1; i < match.size(); i++)
            {
                std::string quoted_string = match[i].str();
                result.push_back(quoted_string);
            }
        }
        return result;
    }

    void load(String file)
    {
        std::ifstream myfile;
        myfile.open(file.c_str());

        String line;
        Bool closed = true;
        Bool entry = false;
        I32 id = -1;
        I32 source = -1;
        I32 target = -1;
        NodeType type = NodeType::None;
        String label;
        String action;
        containers::Map<I32, State> state_ids;

        current_state = "";
        while (std::getline(myfile, line))
        {            
            if (line.rfind("create") == 0)
            {
                if (!closed)
                {
                    closed = true;
                    if (type == NodeType::State)
                    {
                        define_state(label);
                        state_ids[id] = label;

                        if (current_state == "") current_state = label;
                    }
                    else if (type == NodeType::Link)
                    {
                        define_transitions(state_ids[source], label, action, state_ids[target]);
                    }

                    if (entry)
                    {
                        current_state = label;
                    }
                    entry = false;
                }
            }

            if (line.rfind("create-dot") == 0)
            {
                auto m = match(line, "create-dot\\s+(\\d+)");
                type = NodeType::State;
                id = std::atoi(m[0].c_str());
                closed = false;
            }
            else if (line.rfind("create-link") == 0)
            {
                auto m = match(line, "create-link\\s+(\\d+)");
                type = NodeType::Link;
                id = std::atoi(m[0].c_str());
                closed = false;
            }
            else if (line.rfind("create-label") == 0)
            {
                closed = true;
            }
            else if (line.rfind("set-source") == 0)
            {
                if (closed) continue;
                auto m = match(line, "set-source\\s+(\\d+)\\s+(\\d+)");
                source = std::atoi(m[1].c_str());
            }
            else if (line.rfind("set-target") == 0)
            {
                if (closed) continue;
                auto m = match(line, "set-target\\s+(\\d+)\\s+(\\d+)");
                target = std::atoi(m[1].c_str());
            }
            else if (line.rfind("add-tag") == 0)
            {
                if (closed) continue;
                auto m = match(line, "add-tag\\s+(\\d+)\\s+\\\"([^\\\"]*)\\\"\\s+\\\"([^\\\"]*)\\\"");
                auto& key = m[1];
                auto& val = m[2];
                if (key == "Action") 
                {
                    define_action(val);
                    action = val;
                }
                else if (key == "Label") 
                {
                    label = val;
                }
                else if (key == "Entry")
                {
                    entry = true;
                }
            }
        }

        if (!closed)
        {
            closed = true;
            if (type == NodeType::State)
            {
                define_state(label);
                state_ids[id] = label;

                if (current_state == "") current_state = label;
            }
            else if (type == NodeType::Link)
            {
                define_transitions(state_ids[source], label, action, state_ids[target]);
            }

            if (entry)
            {
                current_state = label;
            }
            entry = false;
        }

        myfile.close();
    }

    FSM(String file) 
    {
        load(file);
    }
};
