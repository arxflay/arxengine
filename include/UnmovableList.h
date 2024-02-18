#pragma once
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

};

ARX_NAMESPACE_END
