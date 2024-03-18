#ifndef ARX_ARXOBJECT_H
#define ARX_ARXOBJECT_H
#include "ArxDefines.h"
#include "arxengine/evt/EventManager.h"
#include "arxengine/misc/UnmovableList.h"
#include <list>

ARX_NAMESPACE_BEGIN

class ArxObject;
using ArxObjectList = UnmovableList<ArxObject*>;

class ARX_EXPORTS ArxObject
{
public:
    virtual ~ArxObject();
    ArxObject(ArxObject *parent = nullptr);

    ArxObject(ArxObject &&obj) = delete;
    ArxObject(const ArxObject &obj) = delete;
    ArxObject &operator=(ArxObject &&obj) = delete;
    ArxObject &operator=(const ArxObject &obj) = delete;

    const ArxObjectList &GetChildren() const;

    ArxObject *GetParent();
    const ArxObject *GetParent() const; 
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

#endif
