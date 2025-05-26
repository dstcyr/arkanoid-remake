#pragma once

#include <map>
#include <typeindex>
#include <functional>
#include <memory>
#include <list>

class Event
{};

class EventCallbackBase
{
private:
    virtual void InternalCall(const Event& e) = 0;

public:
    virtual ~EventCallbackBase() = default;
    void Invoke(const Event& e);
};

template <typename TObject, typename TEvent>
class EventCallback : public EventCallbackBase
{
private:
    using CallbackFunction = void (TObject::*)(const TEvent&);

    TObject* m_userObject;
    CallbackFunction m_callback;

    void InternalCall(const Event& e) override;

public:
    virtual ~EventCallback() override = default;

    EventCallback(TObject* userObject, CallbackFunction callback);
};

class CDelegate
{
public:
    using CallbackList = std::list<std::unique_ptr<EventCallbackBase>>;
    using CallbackMap = std::map<std::type_index, std::unique_ptr<CallbackList>>;

    void Clear();

    template <typename TObject, typename TEvent>
    void Bind(TObject* userObject, void (TObject::* callback)(const TEvent&));

    template <typename TEvent, typename ...TArgs>
    void Invoke(TArgs&& ...args);

private:
    CallbackMap m_callbacks;
};

template<typename TObject, typename TEvent>
inline void CDelegate::Bind(TObject* userObject, void(TObject::* callback)(const TEvent&))
{
    const type_info& id = typeid(TEvent);
    if (!m_callbacks[id])
    {
        m_callbacks[id] = std::make_unique<CallbackList>();
    }

    auto delegate = std::make_unique<EventCallback<TObject, TEvent>>(userObject, callback);
    m_callbacks[id]->push_back(std::move(delegate));
}

template<typename TEvent, typename ...TArgs>
inline void CDelegate::Invoke(TArgs && ...args)
{
    const type_info& id = typeid(TEvent);
    if (m_callbacks[id])
    {
        for (auto& it : *m_callbacks[id])
        {
            TEvent event(std::forward<TArgs>(args)...);
            it->Invoke(event);
        }
    }
}

template<typename TObject, typename TEvent>
inline void EventCallback<TObject, TEvent>::InternalCall(const Event& e)
{
    std::invoke(m_callback, m_userObject, static_cast<const TEvent&>(e));
}

template<typename TObject, typename TEvent>
inline EventCallback<TObject, TEvent>::EventCallback(TObject* userObject, CallbackFunction callback)
{
    m_userObject = userObject;
    this->m_callback = callback;
}
