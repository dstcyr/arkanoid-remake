#pragma once
#include <map>
#include <typeindex>
#include <functional>
#include <memory>
#include <list>
#include <vector>

class CTaskState
{};

class CTaskCallbackBase
{
private:
    virtual bool InternalCall(float dt, CTaskState* e) = 0;

public:
    virtual ~CTaskCallbackBase() = default;
    bool Invoke(float dt, CTaskState* e);
    virtual bool Update(float dt) = 0;
};

template<typename TObject, typename TState>
class CTaskCallback : public CTaskCallbackBase
{
private:
    using CallbackFunction = bool (TObject::*)(float, TState*);

    TObject* m_userObject;
    TState* m_stateObject;
    CallbackFunction m_callback;

    bool InternalCall(float dt, CTaskState* e) override;

public:
    virtual ~CTaskCallback() override = default;
    CTaskCallback(TObject* userObject, CallbackFunction callback);
    CTaskCallback(TObject* userObject, TState* state, CallbackFunction callback);
    bool Update(float dt) override;
};

class CTaskManager
{
public:
    using TaskCallbackList = std::list<CTaskCallbackBase*>;

    void Clear();
    void Update(float dt);
    void UpdateSequence(float dt);
    bool HasTasks() const;

    template<typename TObject, typename TState>
    void Add(TObject* userObject, TState* state, bool(TObject::* callback)(float, TState*));

    template<typename TObject, typename TState>
    void Add(TObject* userObject, bool(TObject::* callback)(float, TState*));

private:
    TaskCallbackList m_taskList;
    TaskCallbackList m_taskToDelete;
};

template<typename TObject, typename TState>
inline bool CTaskCallback<TObject, TState>::InternalCall(float dt, CTaskState* e)
{
    auto r = std::invoke(m_callback, m_userObject, dt, static_cast<TState*>(e));
    return r;
}

template<typename TObject, typename TState>
inline CTaskCallback<TObject, TState>::CTaskCallback(TObject* userObject, CallbackFunction callback)
{
    m_userObject = userObject;
    m_stateObject = new TState();
    m_callback = callback;
}

template<typename TObject, typename TState>
inline CTaskCallback<TObject, TState>::CTaskCallback(TObject* userObject, TState* state, CallbackFunction callback)
{
    m_userObject = userObject;
    m_stateObject = state;
    m_callback = callback;
}

template<typename TObject, typename TState>
inline bool CTaskCallback<TObject, TState>::Update(float dt)
{
    return InternalCall(dt, m_stateObject);
}

template<typename TObject, typename TState>
inline void CTaskManager::Add(TObject* userObject, TState* state, bool(TObject::* callback)(float, TState*))
{
    auto newTask = new CTaskCallback<TObject, TState>(userObject, state, callback);
    m_taskList.push_back(newTask);
}

template<typename TObject, typename TState>
inline void CTaskManager::Add(TObject* userObject, bool(TObject::* callback)(float, TState*))
{
    auto newTask = new CTaskCallback<TObject, TState>(userObject, callback);
    m_taskList.push_back(newTask);
}





























//
//class CTaskManager
//{
//public:
//    bool HasTasks() const;
//    void Update(float dt);
//    void Clear();
//
//    template<typename TObject>
//    inline void Add(TObject* userObject, bool(TObject::* callback)(float));
//
//private:
//    std::vector<std::function<bool(float)>> m_tasks;
//};
//
//template<typename TObject>
//inline void CTaskManager::Add(TObject* userObject, bool(TObject::* callback)(float))
//{
//    auto func = std::bind(callback, userObject, std::placeholders::_1);
//    m_tasks.push_back(func);
//}
