#include "arxengine/ui/Monitor.h"
#include "arxengine/misc/Math.h"
#include <GLFW/glfw3.h>

ARX_NAMESPACE_BEGIN

SizeI ResolutionMode::GetSize() const
{
    SizeI size;
    size.height = m_vidMode->height;
    size.width = m_vidMode->width;
    return size;
}

int ResolutionMode::GetRefreshRate() const
{
    return m_vidMode->refreshRate;
}
    
ResolutionMode::ResolutionMode(const GLFWvidmode *vidMode)
    : m_vidMode(vidMode)
{
}

Monitor Monitor::InitPrimaryMonitor()
{
    Monitor m;
    m.m_monitor = glfwGetPrimaryMonitor();
    return m;
}

ResolutionMode Monitor::GetResolutionMode() const
{
    ResolutionMode mode(glfwGetVideoMode(m_monitor));
    return mode;
}

const Monitor &Monitor::GetPrimaryMonitor()
{
    static Monitor monitor = InitPrimaryMonitor();
    return monitor;
}

Monitor::ResolutionFilter::ResolutionFilter()
    : m_retrieveWithMaxRefreshRate(false)
    , m_aspectRatioNumerator(0)
    , m_aspectRatioDenominator(0)
{
    
}

Monitor::ResolutionFilter &Monitor::ResolutionFilter::SetRetrieveWithMaxRefreshRate(bool retrieveWithMaxRefreshRate)
{
    m_retrieveWithMaxRefreshRate = retrieveWithMaxRefreshRate;
    return *this;
}

bool Monitor::ResolutionFilter::GetRetrieveWithMaxRefreshRate() const
{
    return m_retrieveWithMaxRefreshRate;
}

Monitor::ResolutionFilter &Monitor::ResolutionFilter::SetAspectRatio(uint8_t numerator, uint8_t denominator)
{
    m_aspectRatioNumerator = numerator;
    m_aspectRatioDenominator = denominator;
    return *this;
}

Size<uint8_t> Monitor::ResolutionFilter::GetAspectRatio() const
{
    return Size<uint8_t>(m_aspectRatioNumerator, m_aspectRatioDenominator);
}

std::vector<ResolutionMode> Monitor::GetResolutions(const ResolutionFilter &filter) const
{
    int count = 0;
    const GLFWvidmode *mode = glfwGetVideoModes(m_monitor, &count);
    const GLFWvidmode *end = mode + count;
    std::vector<ResolutionMode> resolutions;
    resolutions.reserve(static_cast<size_t>(count));
    Size<uint8_t> aspectRatioFilter = filter.GetAspectRatio();
    bool validAspectRatio = aspectRatioFilter.width != 0 && aspectRatioFilter.height != 0;
    float aspectRatio = 0;
    if (validAspectRatio)
        aspectRatio = static_cast<float>(aspectRatioFilter.width) / static_cast<float>(aspectRatioFilter.height);

    for (; mode != end; mode++)
    {
        if (filter.GetRetrieveWithMaxRefreshRate())
        {
            const GLFWvidmode *nextMode = mode + 1;
            if (nextMode != end && nextMode->width == mode->width && nextMode->height == mode->height)
                continue;
        }
        if (validAspectRatio)
        {
            float modeRatio = static_cast<float>(mode->width) / static_cast<float>(mode->height);
            if(!AreNumsEqual<float>(modeRatio, aspectRatio))
                continue;
        }

        resolutions.push_back(ResolutionMode(mode));
    }
    resolutions.shrink_to_fit();

    return resolutions;
}
    
ARX_NAMESPACE_END
