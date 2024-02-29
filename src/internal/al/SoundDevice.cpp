#include "internal/al/SoundDevice.h"
#include <AL/alc.h>
ARX_NAMESPACE_BEGIN

SoundContext::SoundContext()
    : m_context(nullptr, alcDestroyContext)
{
}

SoundContext::SoundContext(SoundDevice &device)
    : SoundContext()
{
    if (!device.IsInvalid())
        m_context.reset(alcCreateContext(device.m_device.get(), nullptr));
}


void SoundContext::Bind()
{
    alcMakeContextCurrent(m_context.get());
}

void SoundContext::Unbind()
{
    if (IsBound())
        alcMakeContextCurrent(nullptr);
}

bool SoundContext::IsBound()
{
    return (m_context.get() == alcGetCurrentContext());
}

bool SoundContext::IsInvalid()
{
    return !m_context.get();
}

SoundContext::~SoundContext()
{
    Unbind();
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
    : m_device(nullptr, alcCloseDevice)
{
    m_device.reset(alcOpenDevice(nullptr));
}

SoundDevice::SoundDevice(std::string_view deviceName)
    : m_device(nullptr, alcCloseDevice)
{
    m_device.reset(alcOpenDevice(deviceName.data()));
}

bool SoundDevice::IsInvalid()
{
    return !m_device.get();
}

SoundContext SoundDevice::CreateContext()
{
    return SoundContext(*this);
}

ARX_NAMESPACE_END
