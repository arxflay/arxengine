#include <type_traits>
#include "arxengine/ArxDefines.h"

ARX_NAMESPACE_BEGIN

template<typename T> 
using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;

template<typename T, typename T2>
constexpr bool is_not_same_v = !std::is_same_v<T, T2>;

ARX_NAMESPACE_END
