#include "TaskManager.h"

bool CTaskCallbackBase::Invoke(float dt, CTaskState* e)
{
    return InternalCall(dt, e);
}

void CTaskManager::Clear()
{
    auto it = m_taskList.begin();
    while (it != m_taskList.end())
    {
        m_taskToDelete.push_back(*it);
        it++;
    }
}

void CTaskManager::Update(float dt)
{
    auto it = m_taskList.begin();
    while (it != m_taskList.end())
    {
        bool r = (*it)->Update(dt);

        if (!r)
        {
            it = m_taskList.erase(it);
        }
        else
        {
            it++;
        }
    }

    if (m_taskToDelete.size() > 0)
    {
        auto it = m_taskToDelete.begin();
        while (it != m_taskToDelete.end())
        {
            auto taskIt = m_taskList.begin();
            while (taskIt != m_taskList.end())
            {
                if (*it == *taskIt)
                {
                    taskIt = m_taskList.erase(taskIt);
                }
                else
                {
                    taskIt++;
                }
            }

            delete *it;
            it++;
        }

        m_taskToDelete.clear();
    }
}

void CTaskManager::UpdateSequence(float dt)
{
    if (!m_taskList.empty())
    {
        if (!m_taskList.front()->Update(dt))
        {
            auto taskIt = m_taskList.begin();
            while (taskIt != m_taskList.end())
            {
                if ((*taskIt) == m_taskList.front())
                {
                    m_taskList.erase(taskIt);
                    return;
                }

                taskIt++;
            }
        }
    }
}

bool CTaskManager::HasTasks() const
{
    return m_taskList.size() > 0;
}













//
//
//bool CTaskManager::HasTasks() const
//{
//    return !m_tasks.empty();
//}
//
//void CTaskManager::Update(float dt)
//{
//    if (HasTasks())
//    {
//        auto it = m_tasks.begin();
//        if (!(*it)(dt))
//        {
//            if (it != m_tasks.end())
//            {
//                m_tasks.erase(it);
//            }
//        }
//    }
//}
//
//void CTaskManager::Clear()
//{
//    m_tasks.clear();
//}
