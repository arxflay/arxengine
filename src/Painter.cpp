#include "Painter.h"
#include "UIObject.h"
#include <glad/glad.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "ArxWindow.h"
#include "internal/gl/Scissorbox.h"
#include "ui/internal/UICache.h"

ARX_NAMESPACE_BEGIN

Painter::Painter(DrawEvent &evt)
    : m_sender(dynamic_cast<UIObject*>(evt.GetSender()))
{
}

void Painter::DrawRectangle(Position pos, Size size)
{
    Position senderPos = GetSenderPosition();
    Size clientSize = GetSender()->GetClientSize();
    ClipGuard clipGuard(senderPos, clientSize);
    OldVBOGuard vboGuard;
    OldVAOGuard vaoGuard;
    pos = CalculateDrawPosition(pos, size); 
    
    VAO &rectVao = GetUICache()->GetVAOMap().at(UICache::VAO_ID::RECTANGLE);
    rectVao.Bind();
    Shader &shader = GetUICache()->GetShaderMap().at(UICache::SHADER_PROGRAM_ID::RECTANGLE);
    shader.UseShader();
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(pos.x, pos.y, 0.0f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(size.width, size.height, 0.0f));
    shader.SetUniformMat4("modelMatrix", modelMatrix);
    glm::mat4 viewMatrix = glm::mat4(1.0f);
    shader.SetUniformMat4("viewMatrix", viewMatrix);
    shader.SetUniformMat4("projectionMatrix", GetViewport());
    shader.SetUniformVec4("color", glm::vec4(1.0f, 0.0f, 1.0f, 1.0f));

    glDrawArrays(GL_TRIANGLES, 0, 6); 
}

Position Painter::CalculateDrawPosition(Position uiobjectPos, Size uiobjectSize)
{
    Position senderPos = GetSenderPosition();
    ArxWindow *window = Painter::GetWindow();
    Size windowClientSize = window->GetClientSize();
    uiobjectPos.y = -uiobjectPos.y;
    uiobjectPos.y += windowClientSize.height;
    uiobjectPos.y -= uiobjectSize.height;
    uiobjectPos += senderPos;
    
    return uiobjectPos;
}

Position Painter::GetSenderPosition()
{
    //if ownerWindow is nullptr then it is MainWindow
    return (m_sender->GetOwnerWindow() == nullptr) ? Position(0, 0) : m_sender->GetPosition(); 
}

const glm::mat4 &Painter::GetViewport()
{
    return GetWindow()->GetViewport(); 
}

UIObject *Painter::GetSender()
{
    return m_sender;
}

ArxWindow *Painter::GetWindow()
{
    return (m_sender->GetOwnerWindow() == nullptr) ? static_cast<ArxWindow*>(m_sender) : m_sender->GetOwnerWindow();
}

UICache *Painter::GetUICache()
{
    return GetWindow()->GetUICache();
}

ARX_NAMESPACE_END
