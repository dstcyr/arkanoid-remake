#include "Delegate.h"

void CDelegate::Clear()
{
    m_callbacks.clear();
}

void EventCallbackBase::Invoke(const Event& e)
{
    InternalCall(e);
}
