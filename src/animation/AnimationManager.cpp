#include "arxengine/animation/AnimationManager.h"
#include "arxengine/animation/Animation.h"

ARX_NAMESPACE_BEGIN

AnimationManager::AnimationManager(arx::ArxObject *parent)
    : arx::ArxObject(parent)
    , m_currentAnimation(nullptr)
    , m_isRunning(false)
{
}

bool AnimationManager::Load(std::string_view id)
{
    if (m_currentAnimation)
    {
        m_currentAnimation->Stop();
        m_currentAnimation = nullptr;
    }

    auto it = m_animations.find(id.data());
    if (it != m_animations.end())
    {
        m_currentAnimation = it->second;
        m_currentAnimationName = id;
        return true;
    }

    return false;
}

bool AnimationManager::LoadAndRun(std::string_view id, bool loop)
{
    bool status = Load(id);
    Run(loop);

    return status;
}

void AnimationManager::Run(bool loop)
{
    if (m_currentAnimation)
    {
        m_currentAnimation->Run(loop);
        m_isRunning = true;
    }
}

bool AnimationManager::IsRunning() const
{
    return m_isRunning;
}

void AnimationManager::Stop()
{
    if (m_currentAnimation)
        m_currentAnimation->Stop();

    m_isRunning = false;
}

void AnimationManager::ClaimAnimation(std::string_view id, Animation *animation)
{
    animation->Reparent(this);
    ReplaceOldAnimationIfExists(id, animation);
}

void AnimationManager::ReplaceOldAnimationIfExists(std::string_view id, Animation *newAnimation)
{
    auto it = m_animations.find(id.data());
    if (it != m_animations.end())
    {
        it->second->Stop();
        it->second->RequestDelete();
        it->second = newAnimation;
    }
    else
        m_animations.emplace(id, newAnimation);
}

std::string_view AnimationManager::GetCurrentAnimationName()
{
    return m_currentAnimation ? std::string_view(m_currentAnimationName) : "";
}

ARX_NAMESPACE_END
