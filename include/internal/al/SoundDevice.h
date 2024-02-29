#ifndef ARX_SOUNDDEVICE_H
#define ARX_SOUNDDEVICE_H
#include "ArxDefines.h"
#include <string_view>
#include <memory>

class ALCdevice;
class ALCcontext;

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
    std::unique_ptr<ALCcontext, void(*)(ALCcontext*)>  m_context;
};

class SoundDevice final 
{
friend class SoundContext;
public:
    SoundDevice();
    SoundDevice(std::string_view deviceName);
    SoundContext CreateContext();
    bool IsInvalid();

private:
        std::unique_ptr<ALCdevice, char(*)(ALCdevice*)>  m_device;
};

    
ARX_NAMESPACE_END

#endif
