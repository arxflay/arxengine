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
#include "arxengine/gl/Shader.h"
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
        SizeF senderSize = obj->GetClientSize();
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
            senderSize.height = std::clamp(senderSize.height, 0.0f, parentSize.height - relativeSenderPos.y);
        }
        const SizeF &windowSize = window->GetClientSize();
        const SizeF &viewPortSize = window->GetViewport().size;
        
        //invert y axis, top = 0
        Position clipPosition = senderPos;
        

        SizeF clipSize(senderSize.width, senderSize.height);
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

    IShader &GetShader(UICache::SHADER_PROGRAM_ID programId, std::optional<Shader*> userShader, UICache *cache)
    {
        if (userShader.has_value())
            return *userShader.value();

        return cache->GetShaderMap().at(programId);
    }

}

Painter::Painter(DrawEvent &evt)
    : m_sender(dynamic_cast<UIControl*>(evt.GetSender()))
    , m_brush(m_sender->GetColor())
    , m_pen(constants::COLOR_BLACK)
    , m_trasparencyFactor(1.0f)
{
    if (m_sender->IsEnabledClipToBounds())
    {     
        m_clippingArea = std::make_unique<ClippingArea>();
        m_clippingArea->SetClipBox(CreateClipBox(m_sender));
    }

}

void Painter::DrawRectangle(const Position &pos, const SizeF &size)
{
    glEnable(GL_BLEND);
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
        const Position &cameraPos = GetSender()->GetWindow()->GetCameraPos();
        viewMatrix =  glm::translate(viewMatrix, glm::vec3(-cameraPos.x, -cameraPos.y, 0.0f));
    }
        
    VAO &rectVao = GetUICache(m_sender)->GetVAOMap().at(UICache::VAO_ID::RECTANGLE);
    rectVao.Bind();
    IShader &shader = GetShader(UICache::SHADER_PROGRAM_ID::RECTANGLE, m_userShader, GetUICache(m_sender));
    shader.UseShader();
    shader.SetTransformMatrices(modelMatrix, viewMatrix, GetViewport().projectionMatrix);
    shader.SetUniformVec4("color", m_brush.GetColor().GetNormalizedColorRGBA());
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisable(GL_BLEND);
}

void Painter::DrawTexture2D(const Position &pos, const SizeF &size, const Texture2D *tex, int tileWidthCount, int tileHeightCount)
{
    if (!tex)
        throw ArxException(ArxException::ErrorCode::GenericError, "Painter: Texture2D texture is null");
    else if (tex->GetWindow() != GetSender()->GetWindow())    
        throw ArxException(ArxException::ErrorCode::GenericError, "Painter: Texture2D owner mismatch");

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
        const Position &cameraPos = GetSender()->GetWindow()->GetCameraPos();
        viewMatrix =  glm::translate(viewMatrix, glm::vec3(-cameraPos.x, -cameraPos.y, 0.0f));
    }

    tex->Bind();
    VAO &rectVao = GetUICache(m_sender)->GetVAOMap().at(UICache::VAO_ID::RECTANGLE);
    rectVao.Bind();
    IShader &shader = GetShader(UICache::SHADER_PROGRAM_ID::IMAGE, m_userShader, GetUICache(m_sender));
    shader.UseShader();
    shader.SetTransformMatrices(modelMatrix, viewMatrix, GetViewport().projectionMatrix);
    shader.SetUniformInt("tileHeightCount", tileHeightCount);
    shader.SetUniformInt("tileWidthCount", tileWidthCount);
    shader.SetUniformFloat("transparencyFactor", m_trasparencyFactor);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisable(GL_BLEND);
}

void arx::Painter::RenderText(std::string_view text, const Position &pos)
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
        const Position &cameraPos = GetSender()->GetWindow()->GetCameraPos();
        viewMatrix = glm::translate(viewMatrix, glm::vec3(-cameraPos.x, -cameraPos.y, 0.0f));
    }
    
    VAO &rectVao = GetUICache(m_sender)->GetVAOMap().at(UICache::VAO_ID::RECTANGLE);
    rectVao.Bind();
    IShader &shader = GetShader(UICache::SHADER_PROGRAM_ID::TEXT, m_userShader, GetUICache(m_sender));
    shader.UseShader();
    shader.SetUniformVec4("color", m_pen.GetColor().GetNormalizedColorRGBA());

    Position drawingPos = CalculateDrawPosition(pos, SizeF(0.0f, 0.0f));
    float originalDrawPosX = drawingPos.x;
    auto *fontCache = m_sender->GetFontCache();
    float maxHeight = 0;
    for (char ch : text)
    {
        const auto &cacheEntry = fontCache->GetCacheEntry(ch);
        if (ch == '\n')
        {
            drawingPos.x = originalDrawPosX;
            drawingPos.y -= static_cast<float>(maxHeight + 1);
            maxHeight = static_cast<float>(cacheEntry.GetGlyphDimensions().size.height);
            continue;
        }

        maxHeight = std::max(maxHeight, static_cast<float>(cacheEntry.GetGlyphDimensions().size.height) + static_cast<float>(cacheEntry.GetGlyphDimensions().size.height) - cacheEntry.GetGlyphDimensions().bearings.y);
        if (drawingPos.x != originalDrawPosX)
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

void Painter::SetUserShader(std::optional<Shader*> m_shader)
{   
    if (m_shader.has_value() && m_shader.value()->GetWindow() != GetSender()->GetWindow())
        throw ArxException(ArxException::ErrorCode::GenericError, "Painter: Shader owner mismatch");
    m_userShader = m_shader;
}

std::optional<Shader*> Painter::GetUserShader() const
{
    return m_userShader;
}

Position Painter::CalculateDrawPosition(const Position &originalDrawPos, const SizeF &originalDrawSize)
{
    Position senderPos = GetSenderPosition(m_sender); //draw offset, because sender could be positioned somewhereElse
    ArxWindow *window = m_sender->GetWindow();
    SizeF viewPortSize = window->GetViewport().size;
    Position newDrawPos; 
    //inverse top axis, top should be 0 and bottom == viewPortSize.size()
    newDrawPos.y = -originalDrawPos.y;
    newDrawPos.y += viewPortSize.height;
    newDrawPos.y -= originalDrawSize.height;
    newDrawPos.x = senderPos.x + originalDrawPos.x;
    newDrawPos.y -= senderPos.y;

    return newDrawPos;
}

void Painter::Clear()
{
    OldClippingAreaGuard clipGuard;
    if (m_clippingArea)
        m_clippingArea->UseThisClippingArea();
    
    glm::vec4 normalizedColor = m_brush.GetColor().GetNormalizedColorRGBA();
    glClearColor(normalizedColor.r, normalizedColor.g, normalizedColor.b, 1.0f);
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

void Painter::SetTextureTransparencyFactor(float factor)
{
    m_trasparencyFactor = factor;
}

float Painter::GetTextureTransparencyFactor() const
{
    return m_trasparencyFactor;
}

Painter::~Painter() = default;

ARX_NAMESPACE_END
