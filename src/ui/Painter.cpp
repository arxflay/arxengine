#include "ui/Painter.h"
#include "ui/UIObject.h"
#include <glad/glad.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "ui/ArxWindow.h"
#include "internal/gl/Scissorbox.h"
#include "ui/internal/UICache.h"
#include "gl/Texture2D.h"

ARX_NAMESPACE_BEGIN
namespace
{
    ArxWindow *GetWindow(UIObject *obj)
    {
        return (obj->GetOwnerWindow() == nullptr) ? static_cast<ArxWindow*>(obj) : obj->GetOwnerWindow(); 
    }

    UICache *GetUICache(UIObject *obj)
    {
        return GetWindow(obj)->GetUICache();
    }

    Position GetPositionForDraw(UIObject *obj)
    {
        //if ownerWindow is nullptr then it is MainWindow
        return (obj->GetOwnerWindow() == nullptr) ? Position(0, 0) : obj->GetPosition(); 
    }

    ClipGuard CreateClipGuard(UIObject *obj)
    {
        Position clipPosition (GetPositionForDraw(obj).x, (GetWindow(obj)->GetClientSize().height - obj->GetClientSize().height) - GetPositionForDraw(obj).y);
        ClipGuard clipGuard(clipPosition, obj->GetClientSize());
        return clipGuard;
    }
}

Painter::Painter(DrawEvent &evt)
    : m_sender(dynamic_cast<UIObject*>(evt.GetSender()))
    , m_brush(m_sender->GetColor())
{
}

void Painter::DrawRectangle(Position pos, Size size)
{
    ClipGuard clipGuard(CreateClipGuard(m_sender)); 
    OldVAOGuard vaoGuard;
    pos = CalculateDrawPosition(pos, size); 
    
    VAO &rectVao = GetUICache(m_sender)->GetVAOMap().at(UICache::VAO_ID::RECTANGLE);
    rectVao.Bind();
    Shader &shader = GetUICache(m_sender)->GetShaderMap().at(UICache::SHADER_PROGRAM_ID::RECTANGLE);
    shader.UseShader();
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(pos.x, pos.y, 0.0f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(size.width, size.height, 0.0f));
    glm::mat4 viewMatrix = glm::mat4(1.0f);
    shader.SetTransformMatrices(modelMatrix, viewMatrix, GetViewport());
    shader.SetUniformVec4("color", m_brush.GetColor().GetNormalizedColorRGBA());
    glDrawArrays(GL_TRIANGLES, 0, 6); 
}

void Painter::DrawTexture2D(Position pos, Size size, const Texture2D *tex)
{
    ClipGuard clipGuard(CreateClipGuard(m_sender)); 
    OldVAOGuard vaoGuard;
    pos = CalculateDrawPosition(pos, size);

    tex->Bind();
    VAO &rectVao = GetUICache(m_sender)->GetVAOMap().at(UICache::VAO_ID::RECTANGLE);
    rectVao.Bind();
    Shader &shader = GetUICache(m_sender)->GetShaderMap().at(UICache::SHADER_PROGRAM_ID::IMAGE);
    shader.UseShader();
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(pos.x, pos.y, 0.0f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(size.width, size.height, 0.0f));
    glm::mat4 viewMatrix = glm::mat4(1.0f);
    shader.SetTransformMatrices(modelMatrix, viewMatrix, GetViewport());
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

Position Painter::CalculateDrawPosition(Position uiobjectPos, Size uiobjectSize)
{
    Position senderPos = GetPositionForDraw(m_sender);
    ArxWindow *window = GetWindow(m_sender);
    Size windowClientSize = window->GetClientSize();
    uiobjectPos.y = -uiobjectPos.y;
    uiobjectPos.y += windowClientSize.height;
    uiobjectPos.y -= uiobjectSize.height;
    uiobjectPos += senderPos;
    
    return uiobjectPos;
}

void Painter::Clear()
{
    ClipGuard clipGuard(CreateClipGuard(m_sender)); 
    OldVAOGuard vaoGuard;
    glm::vec4 normalizedColor = m_brush.GetColor().GetNormalizedColorRGBA();
    glClearColor(normalizedColor.r, normalizedColor.g, normalizedColor.b, normalizedColor.a);
    glClear(GL_COLOR_BUFFER_BIT);
}

const glm::mat4 &Painter::GetViewport()
{
    return GetWindow(m_sender)->GetViewport(); 
}

UIObject *Painter::GetSender()
{
    return m_sender;
}

void Painter::SetBrush(const Brush &brush)
{
    m_brush = brush;
}

const Brush &Painter::GetBrush() const
{
    return m_brush;
}

ARX_NAMESPACE_END
