#include "internal/al/SoundDevice.h"
#include <AL/alc.h>
#include <logging/FileLogger.h>
ARX_NAMESPACE_BEGIN

SoundContext::SoundContext()
    : m_context(nullptr)
{
}

SoundContext::SoundContext(SoundDevice &device)
    : SoundContext()
{
    if (!device.IsInvalid())
        m_context = alcCreateContext(device.m_device, nullptr);
    ALCenum error = alcGetError(device.m_device);
    if (error != ALC_NO_ERROR)
        GLOG->Debug("Error occured during SoundContext creation, AlcErrNum %d", error);
}


void SoundContext::Bind()
{
    alcMakeContextCurrent(m_context);
}

void SoundContext::Unbind()
{
    if (IsBound())
        alcMakeContextCurrent(nullptr);
}

bool SoundContext::IsBound()
{
    return (m_context == alcGetCurrentContext());
}

bool SoundContext::IsInvalid()
{
    return !m_context;
}

SoundContext::~SoundContext()
{
    if (!IsInvalid())
    {
        Unbind();
        alcDestroyContext(m_context);
    }
}

SoundContext::SoundContext(SoundContext&&ctx)
    : SoundContext()
{
    std::swap(m_context, ctx.m_context);
}

SoundContext &SoundContext::operator=(SoundContext &&ctx)
{
    std::swap(m_context, ctx.m_context);
    return *this;
}

SoundDevice::SoundDevice()
    : m_device(nullptr)
{
    m_device = alcOpenDevice(nullptr);
    ALCenum error = alcGetError(m_device);
    if (error != ALC_NO_ERROR)
        GLOG->Error("Error occured during SoundDevice creation, AlcErrNum %d", error);
}

SoundDevice::SoundDevice(std::string_view deviceName)
    : m_device(nullptr)
{
    m_device = alcOpenDevice(deviceName.data());
}

bool SoundDevice::IsInvalid()
{
    return !m_device;
}

SoundContext SoundDevice::CreateContext()
{
    return SoundContext(*this);
}

SoundDevice::~SoundDevice()
{
    alcCloseDevice(m_device);
}


ARX_NAMESPACE_END
