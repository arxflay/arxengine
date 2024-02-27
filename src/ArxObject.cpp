#include "ArxObject.h"
ARX_NAMESPACE_BEGIN


ArxObject::ArxObject(ArxObject *parent)
    : m_parent(nullptr)
    , m_destroyCalled(false)
    , m_eventManager(this)
{
    Reparent(parent);
}

const ArxObjectList &ArxObject::GetChildren() const
{
    return m_children;
}

ArxObject *ArxObject::GetParent()
{
    return m_parent;
}

void ArxObject::Reparent(ArxObject *parent)
{
    if (!IsDestroyCalled())
    {
        if (parent == this)
            throw ArxException(ArxException::ErrorCode::GenericError, "parent points to this");
        
        if (GetParent())
            m_parent->m_children.remove(this);
        if (parent)
            parent->m_children.push_back(this);

        m_parent = parent;
    }
}

void ArxObject::RequestDelete()
{
    if (!IsDestroyCalled())
    {
        m_destroyCalled = true;

        for (ArxObject *obj : m_children)
            obj->RequestDelete();

        auto evt = std::unique_ptr<DeleteEvent>(new DeleteEvent{});
        m_eventManager.QueueEvent<DeleteEvent>(std::move(evt));
    }
}

bool ArxObject::IsDestroyCalled() const
{
    return m_destroyCalled;
}

EventManager &ArxObject::GetEventManager()
{
    return m_eventManager;        
}

ArxObject::~ArxObject()
{
    if (GetParent())
        m_parent->m_children.remove(this);

    for(ArxObject *obj : m_children)
        if (!obj->IsDestroyCalled())
            delete obj;
}

ARX_NAMESPACE_END
