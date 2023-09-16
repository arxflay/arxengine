#pragma once
#include <memory>
#include <shared_mutex>
#include <mutex>
#include "arxdefines.h"

ARX_NAMESPACE_BEGIN

template<typename T>
class ARX_EXPORTS ThreadsafeQueue
{
private:
    struct node
    {
        node(T &&value)
            : next(nullptr)
            , value(std::make_shared<T>(std::forward<T>(value)))
            
        {
        }
        std::unique_ptr<node> next;
        std::shared_ptr<T> value;
    };
public:
    ThreadsafeQueue()
        : m_head(nullptr), m_tail(nullptr)
    {
    }

    void Enqueue(T &&value)
    {
        std::unique_ptr<node> val(std::make_unique<node>(std::forward<T>(value)));
        node *newTail = val.get();
        std::scoped_lock<std::shared_mutex> lk(m_accessMutex);
        if (m_head == nullptr)
            m_head = std::move(val);
        else
            m_tail->next = std::move(val);
        m_tail = newTail;
    }
    
    std::shared_ptr<T> Dequeue()
    {
        std::scoped_lock<std::shared_mutex> lk(m_accessMutex);
        if (m_head == nullptr)
            return std::shared_ptr<T>(nullptr);
        
        std::unique_ptr<node> oldHead(m_head.release());
        m_head = std::move(oldHead->next);
        if(!m_head.get())
            m_tail = nullptr;

        return oldHead->value;
    }

    bool IsEmpty()
    {
        std::shared_lock<std::shared_mutex> lk(m_accessMutex);
        return m_head == nullptr;
    }

private:
    std::unique_ptr<node> m_head;
    node *m_tail;
    mutable std::shared_mutex m_accessMutex;
};

ARX_NAMESPACE_END
