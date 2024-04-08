/*
 * @author Alexej Fedorenko
 */

#ifndef ARX_MONITOR_H
#define ARX_MONITOR_H
#include "arxengine/ArxDefines.h"
#include "arxengine/ui/Size.h"
#include <vector>

struct GLFWmonitor;
struct GLFWvidmode;

ARX_NAMESPACE_BEGIN

/*
 * @brief GLFW Monitor C++ wrapper 
 */

class ARX_EXPORTS ResolutionMode
{
friend class Monitor;
public:
    SizeI GetSize() const;
    int GetRefreshRate() const;
private:
    ResolutionMode(const GLFWvidmode *monitor);
    const GLFWvidmode *m_vidMode;
};

class ARX_EXPORTS Monitor
{
public:
    class ARX_EXPORTS ResolutionFilter
    {
    public:
        ResolutionFilter();
        ResolutionFilter &SetRetrieveWithMaxRefreshRate(bool retrieveWithMaxRefreshRate);
        bool GetRetrieveWithMaxRefreshRate() const;
        ResolutionFilter &SetAspectRatio(uint8_t numerator, uint8_t denominator);
        Size<uint8_t> GetAspectRatio() const;
    private:
        bool m_retrieveWithMaxRefreshRate;
        uint8_t m_aspectRatioNumerator;
        uint8_t m_aspectRatioDenominator;
    };

    static const Monitor &GetPrimaryMonitor();
    ResolutionMode GetResolutionMode() const;
    
    //return all supported resolutions
    std::vector<ResolutionMode> GetResolutions(const ResolutionFilter &filter = {}) const;
private:
    static Monitor InitPrimaryMonitor();
    Monitor() = default;
    Monitor(Monitor&&) = default;
    Monitor(const Monitor&) = default;
    Monitor &operator=(Monitor&&) = default;
    Monitor &operator=(const Monitor&) = default;
    GLFWmonitor *m_monitor;
};


ARX_NAMESPACE_END

#endif
