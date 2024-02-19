#pragma once
#include "ArxDefines.h"
#include "EventManager.h"
#include "UnmovableList.h"
#include <list>

ARX_NAMESPACE_BEGIN

class ArxObject;
using ArxObjectList = UnmovableList<ArxObject*>;

class ArxObject
{
public:

    virtual ~ArxObject();
    
    ArxObject(ArxObject *parent = nullptr);
    const ArxObjectList &GetChildren() const;

    ArxObject *GetParent(); 
    
    virtual void Reparent(ArxObject *parent);
    
    //create request in event loop
    void RequestDelete();

    bool IsDestroyCalled() const;
    EventManager &GetEventManager();

private:
    ArxObject *m_parent;
    bool m_destroyCalled;
    ArxObjectList m_children;
    EventManager m_eventManager;
};


ARX_NAMESPACE_END
