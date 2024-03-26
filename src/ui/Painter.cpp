#include "arxengine/ui/Painter.h"
#include "arxengine/ui/UIControl.h"
#include <glad/glad.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "arxengine/ui/ArxWindow.h"
#include "arxengine/internal/gl/ClippingArea.h"
#include "arxengine/ui/internal/UICache.h"
#include "arxengine/gl/Texture2D.h"
#include <iostream>

ARX_NAMESPACE_BEGIN
namespace
{
    UICache *GetUICache(UIControl *obj)
    {
        return obj->GetWindow()->GetUICache();
    }

    Position GetSenderPosition(UIControl *obj)
    {
        if (obj->GetParent())
            return obj->GetPosition() + obj->GetParentsPosition();
   
        return Position(0, 0); 
    }

    ClippingArea::ClipBox CreateClipBox(UIControl *obj)
    {
        ArxWindow *window = obj->GetWindow();
        Position senderPos = GetSenderPosition(obj);
        Size senderSize = obj->GetClientSize();
        if (obj->CanBeAffectedByCamera())
        {
            senderPos.x -= window->GetCameraPos().x;
            senderPos.y += window->GetCameraPos().y; //'+' because y axis is inverted
        }

        if (obj->GetParent() != obj->GetOwnerWindow())
        {
            Position relativeSenderPos = obj->GetPosition();
            SizeF parentSize = static_cast<UIControl*>(obj->GetParent())->GetSize();
            senderSize.width = std::clamp(senderSize.width, 0.0f, parentSize.width - relativeSenderPos.x);
            senderSize.height = std::clamp(senderSize.width, 0.0f, parentSize.width - relativeSenderPos.y);
        }
        Size windowSize = window->GetClientSize();
        Size viewPortSize = window->GetViewport().size;
        
        //invert y axis, top = 0
        Position clipPosition = senderPos;
        

        Size clipSize(senderSize.width, senderSize.height);
        if (!(viewPortSize == windowSize))
        {
            //transform screen coordinates and size to viewport dimensions
            float widthCoef = windowSize.width / viewPortSize.width;
            float heightCoef = windowSize.height / viewPortSize.height;
            
            clipPosition.x *= widthCoef;
            clipSize.width *= widthCoef;
            clipSize.height += (clipSize.height * heightCoef) - clipSize.height;
            
            clipPosition.y = -clipPosition.y * heightCoef;
            clipPosition.y += windowSize.height;
            clipPosition.y -= clipSize.height;
        }
        else
        {
            clipPosition.y = -clipPosition.y;
            clipPosition.y += windowSize.height;
            clipPosition.y -= senderSize.height;
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
    : m_sender(dynamic_cast<UIControl*>(evt.GetSender()))
    , m_brush(m_sender->GetColor())
    , m_pen(constants::COLOR_BLACK)
{
    if (m_sender->IsEnabledClipToBounds())
    {     
        m_clippingArea = std::make_unique<ClippingArea>();
        m_clippingArea->SetClipBox(CreateClipBox(m_sender));
    }

}

void Painter::DrawRectangle(Position pos, SizeF size)
{
    OldClippingAreaGuard clipGuard;
    OldVAOGuard vaoGuard;
    if (m_clippingArea)
    {
        m_clippingArea->SetClipBox(CreateClipBox(m_sender));
        m_clippingArea->UseThisClippingArea();
    }

    Position drawPos = CalculateDrawPosition(pos, size);
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(drawPos.x, drawPos.y, 0.0f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(size.width, size.height, 0.0f));
    glm::mat4 viewMatrix = glm::mat4(1.0f);
    if (GetSender()->CanBeAffectedByCamera())
    {
        Position cameraPos = GetSender()->GetWindow()->GetCameraPos();
        viewMatrix =  glm::translate(viewMatrix, glm::vec3(-cameraPos.x, -cameraPos.y, 0.0f));
    }
        
    VAO &rectVao = GetUICache(m_sender)->GetVAOMap().at(UICache::VAO_ID::RECTANGLE);
    rectVao.Bind();
    Shader &shader = GetUICache(m_sender)->GetShaderMap().at(UICache::SHADER_PROGRAM_ID::RECTANGLE);
    shader.UseShader();
    shader.SetTransformMatrices(modelMatrix, viewMatrix, GetViewport().projectionMatrix);
    shader.SetUniformVec4("color", m_brush.GetColor().GetNormalizedColorRGBA());
    glDrawArrays(GL_TRIANGLES, 0, 6); 
}

void Painter::DrawTexture2D(Position pos, SizeF size, const Texture2D *tex, int tileWidthCount, int tileHeightCount)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    OldClippingAreaGuard clipGuard;
    OldVAOGuard vaoGuard;
    if (m_clippingArea)
    {
        m_clippingArea->SetClipBox(CreateClipBox(m_sender));
        m_clippingArea->UseThisClippingArea();
    }

    Position drawingPos = CalculateDrawPosition(pos, size);
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(drawingPos.x, drawingPos.y, 0.0f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(size.width, size.height, 0.0f));
    glm::mat4 viewMatrix = glm::mat4(1.0f);
    if (GetSender()->CanBeAffectedByCamera())
    {
        Position cameraPos = GetSender()->GetWindow()->GetCameraPos();
        viewMatrix =  glm::translate(viewMatrix, glm::vec3(-cameraPos.x, -cameraPos.y, 0.0f));
    }

    tex->Bind();
    VAO &rectVao = GetUICache(m_sender)->GetVAOMap().at(UICache::VAO_ID::RECTANGLE);
    rectVao.Bind();
    Shader &shader = GetUICache(m_sender)->GetShaderMap().at(UICache::SHADER_PROGRAM_ID::IMAGE);
    shader.UseShader();
    shader.SetTransformMatrices(modelMatrix, viewMatrix, GetViewport().projectionMatrix);
    shader.SetUniformInt("tileHeightCount", tileHeightCount);
    shader.SetUniformInt("tileWidthCount", tileWidthCount);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisable(GL_BLEND);
}

void arx::Painter::RenderText(std::string_view text, Position pos)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    OldClippingAreaGuard clipGuard;
    OldVAOGuard vaoGuard;
    if (m_clippingArea)
    {
        m_clippingArea->SetClipBox(CreateClipBox(m_sender));
        m_clippingArea->UseThisClippingArea();
    }

    glm::mat4 viewMatrix = glm::mat4(1.0f);
    if (GetSender()->CanBeAffectedByCamera())
    {
        Position cameraPos = GetSender()->GetWindow()->GetCameraPos();
        viewMatrix = glm::translate(viewMatrix, glm::vec3(-cameraPos.x, -cameraPos.y, 0.0f));
    }
    
    VAO &rectVao = GetUICache(m_sender)->GetVAOMap().at(UICache::VAO_ID::RECTANGLE);
    rectVao.Bind();
    Shader &shader = GetUICache(m_sender)->GetShaderMap().at(UICache::SHADER_PROGRAM_ID::TEXT);
    shader.UseShader();
    shader.SetUniformVec4("color", m_pen.GetColor().GetNormalizedColorRGBA());

    Position drawingPos = CalculateDrawPosition(pos, Size(0.0f, 0.0f));
    auto *fontCache = m_sender->GetFontCache();
    for (char ch : text)
    {
        const auto &cacheEntry = fontCache->GetCacheEntry(ch);
        drawingPos.x += cacheEntry.GetGlyphDimensions().bearings.x;
        if (ch != ' ')
        {
            cacheEntry.GetTexture()->Bind();
            glm::mat4 modelMatrix = glm::mat4(1.0f);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(drawingPos.x, drawingPos.y + cacheEntry.GetGlyphDimensions().bearings.y - static_cast<float>(cacheEntry.GetGlyphDimensions().size.height), 0.0f));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(cacheEntry.GetGlyphDimensions().size.width, cacheEntry.GetGlyphDimensions().size.height, 0.0f));
            shader.SetTransformMatrices(modelMatrix, viewMatrix, GetViewport().projectionMatrix);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        drawingPos.x += cacheEntry.GetGlyphDimensions().advance.x;
    }
    glDisable(GL_BLEND);
}

Position Painter::CalculateDrawPosition(Position drawPos, SizeF drawSize)
{
    Position senderPos = GetSenderPosition(m_sender); //draw offset, because sender could be positioned somewhereElse
    ArxWindow *window = m_sender->GetWindow();
    Size viewPortSize = window->GetViewport().size;
    
    //inverse top axis, top should be 0 and bottom == viewPortSize.size()
    drawPos.y = -drawPos.y;
    drawPos.y += viewPortSize.height;
    drawPos.y -= drawSize.height;
    drawPos.x = senderPos.x + drawPos.x;
    drawPos.y -= senderPos.y;

    
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
    return m_sender->GetWindow()->GetViewport(); 
}

UIControl *Painter::GetSender()
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

void Painter::SetPen(const Pen &pen)
{
    m_pen = pen;
}

const Pen &Painter::GetPen() const
{
    return m_pen;
}

Painter::~Painter() = default;

ARX_NAMESPACE_END
