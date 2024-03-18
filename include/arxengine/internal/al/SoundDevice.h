/*
 * @author Alexej Fedorenko
 */

#ifndef ARX_SOUNDDEVICE_H
#define ARX_SOUNDDEVICE_H
#include "arxengine/ArxDefines.h"
#include <string_view>

struct ALCdevice;
struct ALCcontext;

ARX_NAMESPACE_BEGIN

class SoundDevice;

class SoundContext final
{
friend class SoundDevice;
private:
    SoundContext();
    SoundContext(SoundDevice &device);

public:
    void Bind(); 
    void Unbind();
    bool IsBound();
    bool IsInvalid();
    ~SoundContext();
    SoundContext(SoundContext&&);
    SoundContext &operator=(SoundContext&&);
private:
    SoundContext(const SoundContext&) = delete;
    SoundContext &operator=(const SoundContext&) = delete;
    ALCcontext *m_context;
};

class SoundDevice final 
{
friend class SoundContext;
public:
    SoundDevice();
    ~SoundDevice();
    SoundDevice(std::string_view deviceName);
    SoundContext CreateContext();
    bool IsInvalid();

private:
    ALCdevice *m_device;
};

    
ARX_NAMESPACE_END

#endif
