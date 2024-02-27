#include "ui/Painter.h"
#include "ui/UIObject.h"
#include <glad/glad.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "ui/ArxWindow.h"
#include "internal/gl/ClippingArea.h"
#include "ui/internal/UICache.h"
#include "gl/Texture2D.h"
#include <iostream>

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

    Position GetSenderPosition(UIObject *obj)
    {
        //position is only required if it's an element inside window
        return (obj->GetOwnerWindow() == nullptr) ? Position(0, 0) : obj->GetPosition(); 
    }

    ClippingArea::ClipBox CreateClipBox(UIObject *obj)
    {
        ArxWindow *window = GetWindow(obj);
        Position senderPos = GetSenderPosition(obj);
        Size senderSize = obj->GetClientSize();
        Size windowSize = window->GetClientSize();
        Size viewPortSize = window->GetViewport().size;
        
        //invert y axis, top = 0
        Position clipPosition = senderPos;
        clipPosition.y = -clipPosition.y;
        clipPosition.y += windowSize.height;
        clipPosition.y -= senderSize.height;

        Size clipSize(senderSize.width, senderSize.height);
        if (viewPortSize == windowSize)
        {
            //transform screen coordinates and size to viewport dimensions
            float widthCoef = windowSize.width / viewPortSize.width;
            float heightCoef = windowSize.height / viewPortSize.height;

            clipPosition.x *= widthCoef;
            clipPosition.y *= heightCoef;
            clipSize.width *= widthCoef; 
            clipSize.height *= heightCoef;
        }
        
        //transform size in window to size in Viewport  
        ClippingArea::ClipBox clipBox;
        clipBox.x = static_cast<int>(clipPosition.x);
        clipBox.y = static_cast<int>(clipPosition.y);
        clipBox.width = static_cast<int>(clipSize.width);
        clipBox.height = static_cast<int>(clipSize.height);
        return clipBox;
    }
}

Painter::Painter(DrawEvent &evt)
    : m_sender(dynamic_cast<UIObject*>(evt.GetSender()))
    , m_brush(m_sender->GetColor())
{
    if (m_sender->IsEnabledClipToBounds())
    {     
        m_clippingArea = std::make_unique<ClippingArea>();
        m_clippingArea->SetClipBox(CreateClipBox(m_sender));
    }
}

void Painter::DrawRectangle(Position pos, Size size)
{
    OldClippingAreaGuard clipGuard;
    OldVAOGuard vaoGuard;
    if (m_clippingArea)
        m_clippingArea->UseThisClippingArea();

    ClippingArea clippingArea;
    clippingArea.SetClipBox(CreateClipBox(m_sender));
    clippingArea.UseThisClippingArea();

    Position drawPos = CalculateDrawPosition(pos, size);
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(drawPos.x, drawPos.y, 0.0f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(size.width, size.height, 0.0f));
    glm::mat4 viewMatrix = glm::mat4(1.0f);

    VAO &rectVao = GetUICache(m_sender)->GetVAOMap().at(UICache::VAO_ID::RECTANGLE);
    rectVao.Bind();
    Shader &shader = GetUICache(m_sender)->GetShaderMap().at(UICache::SHADER_PROGRAM_ID::RECTANGLE);
    shader.UseShader();
    shader.SetTransformMatrices(modelMatrix, viewMatrix, GetViewport().projectionMatrix);
    shader.SetUniformVec4("color", m_brush.GetColor().GetNormalizedColorRGBA());
    glDrawArrays(GL_TRIANGLES, 0, 6); 
}

void Painter::DrawTexture2D(Position pos, Size size, const Texture2D *tex)
{
    OldClippingAreaGuard clipGuard;
    OldVAOGuard vaoGuard;
    if (m_clippingArea)
        m_clippingArea->UseThisClippingArea();

    Position drawingPos = CalculateDrawPosition(pos, size);
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(drawingPos.x, drawingPos.y, 0.0f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(size.width, size.height, 0.0f));
    glm::mat4 viewMatrix = glm::mat4(1.0f);
    
    tex->Bind();
    VAO &rectVao = GetUICache(m_sender)->GetVAOMap().at(UICache::VAO_ID::RECTANGLE);
    rectVao.Bind();
    Shader &shader = GetUICache(m_sender)->GetShaderMap().at(UICache::SHADER_PROGRAM_ID::IMAGE);
    shader.UseShader();
    shader.SetTransformMatrices(modelMatrix, viewMatrix, GetViewport().projectionMatrix);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}

Position Painter::CalculateDrawPosition(Position drawPos, Size drawSize)
{
    Position senderPos = GetSenderPosition(m_sender); //draw offset, because sender could be positioned somewhereElse
    ArxWindow *window = GetWindow(m_sender);
    Size viewPortSize = window->GetViewport().size;
    
    //inverse top axis, top should be 0 and bottom == viewPortSize.size()
    drawPos.y = -drawPos.y;
    drawPos.y += viewPortSize.height;
    drawPos.y -= drawSize.height;
    drawPos += senderPos;
    
    return drawPos;
}

void Painter::Clear()
{
    OldClippingAreaGuard clipGuard;
    OldVAOGuard vaoGuard;
    if (m_clippingArea)
        m_clippingArea->UseThisClippingArea();
    
    glm::vec4 normalizedColor = m_brush.GetColor().GetNormalizedColorRGBA();
    glClearColor(normalizedColor.r, normalizedColor.g, normalizedColor.b, normalizedColor.a);
    glClear(GL_COLOR_BUFFER_BIT);
}

const Viewport &Painter::GetViewport()
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

Painter::~Painter() = default;

ARX_NAMESPACE_END
