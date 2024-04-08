/*
 * @author Alexej Fedorenko
 */
#ifndef ARX_ANIMATIOMANAGER_H
#define ARX_ANIMATIOMANAGER_H
#include "arxengine/ArxObject.h"
#include <map>
#include <string_view>

ARX_NAMESPACE_BEGIN

class Animation;

class ARX_EXPORTS AnimationManager : public arx::ArxObject
{
public:
    AnimationManager(arx::ArxObject *parent = nullptr);

    bool Load(std::string_view id);
    bool LoadAndRun(std::string_view id, bool loop);
    void Run(bool loop);
    void Stop();
    bool IsRunning() const;
    void ClaimAnimation(std::string_view id, Animation *animation);
    std::string_view GetCurrentAnimationName();

private:
    void ReplaceOldAnimationIfExists(std::string_view id, Animation *newAnimation);
    Animation *m_currentAnimation;
    std::string m_currentAnimationName;
    std::map<std::string, Animation*> m_animations;
    bool m_isRunning;
}; 


ARX_NAMESPACE_END

#endif
