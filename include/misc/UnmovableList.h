#ifndef ARX_UNMOVABLELIST_H
#define ARX_UNMOVABLELIST_H
#include "ArxDefines.h"
#include <list>
ARX_NAMESPACE_BEGIN

template<typename T>
class UnmovableList : public std::list<T>
{
public:
    UnmovableList() : std::list<T>()
    {
    }

    UnmovableList(const UnmovableList<T>&) = delete;
    UnmovableList(UnmovableList<T>&&) = delete;
    UnmovableList &operator=(const UnmovableList<T>&) = delete;
    UnmovableList &operator=(UnmovableList<T>&&) = delete;

    virtual ~UnmovableList()  = default;

};

ARX_NAMESPACE_END

#endif
