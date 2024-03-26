#ifndef ARX_STATEMACHINE_H
#define ARX_STATEMACHINE_H
#include "arxengine/ArxDefines.h"
#include <cstdint>
#include <functional>
#include <vector>

ARX_NAMESPACE_BEGIN

template<typename RetT, typename InstanceT, typename...Ts>
class StateMachine
{
public:

    StateMachine(unsigned int beginState, std::initializer_list<typename std::vector<std::function<RetT(StateMachine<RetT, InstanceT, Ts...> &, Ts...)>>::value_type> transitions)
        : m_state(beginState), m_transitions(std::move(transitions)), m_functor(m_transitions[m_state])
    {
    }

    void Process(Ts...value)
    {
        m_functor(*this, value...);
    }

    InstanceT *GetInstance()
    {
        return m_instance;
    }

    void SetInstance(InstanceT *instance)
    {
        m_instance = instance;
    }

    void SetState(unsigned int state) { m_state = state; m_functor = m_transitions.at(state); }
    unsigned int GetState() { return m_state; }

private:
    unsigned int m_state;
    InstanceT *m_instance;
    
    std::vector<std::function<RetT(StateMachine<RetT, InstanceT, Ts...> &, Ts...)>> m_transitions;
    std::function<RetT(StateMachine<RetT, InstanceT, Ts...> &, Ts...)> m_functor;
};

template<typename RetT, typename...Ts>
class StateMachine<RetT, void, Ts...>
{
public:

    StateMachine(unsigned int beginState, std::initializer_list<typename std::vector<std::function<RetT(StateMachine<RetT, Ts...> &, Ts...)>>::value_type> transitions)
        : m_transitions(transitions), m_state(beginState), m_functor(m_transitions[m_state])
    {
    }

    void Process(Ts...value)
    {
        m_functor(*this, value...);
    }

    void SetState(unsigned int state) { m_state = state; m_functor = m_transitions.at(state); }
    unsigned int GetState() { return m_state; }

private:
    unsigned int m_state;
    
    std::vector<std::function<RetT(StateMachine<RetT, Ts...> &, Ts...)>> m_transitions;
    std::function<RetT(StateMachine<RetT, Ts...> &, Ts...)> m_functor;
};

template<typename T, typename RetT, typename...Tsfn>
class ObjFnWrapper
{
public:
    ObjFnWrapper(T *instance, RetT (T::*fn)(Tsfn...))
        : m_instance(instance), m_function(fn)
    {
    }

    RetT operator()(Tsfn...parameters)
    {
       return ((*m_instance).*(m_function))(parameters...);
    }

private:
    T *m_instance;
    RetT (T::*m_function)(Tsfn...);
};

template<typename T, typename...Tsfn>
class ObjFnWrapper<T, void, Tsfn...>
{
public:
    ObjFnWrapper(T *instance, void (T::*fn)(Tsfn...))
        : m_instance(instance), m_function(fn)
    {
    }

    void operator()(Tsfn...parameters)
    {
       ((*m_instance).*(m_function))(parameters...);
    }

private:
    T *m_instance;
    void (T::*m_function)(Tsfn...);
};

ARX_NAMESPACE_END

#endif
