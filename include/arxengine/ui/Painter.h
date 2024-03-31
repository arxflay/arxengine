/*
 * @author Alexej Fedorenko
 */

#ifndef ARX_PAINTER_H
#define ARX_PAINTER_H
#include "arxengine/ArxDefines.h"
#include "arxengine/ui/Position.h"
#include "arxengine/ui/Viewport.h"
#include "arxengine/ui/Brush.h"
#include "arxengine/ui/Pen.h"
#include <memory>
#include <optional>

ARX_NAMESPACE_BEGIN
class DrawEvent;
class UIControl;
class Texture2D;
class ClippingArea;
class Shader;
class ARX_EXPORTS Painter final
{
public:
    Painter(DrawEvent &evt);
    void DrawRectangle(const Position &pos, const SizeF &size);
    void DrawTexture2D(const Position &pos, const SizeF &size, const Texture2D *tex, int tileWidthCount = 1, int tileHeightCount = 1);
    void RenderText(std::string_view text, const Position &pos);
    void SetBrush(const Brush &brush);
    void SetPen(const Pen &pen);
    void SetTextureTransparencyFactor(float factor);
    float GetTextureTransparencyFactor() const;

    const Brush &GetBrush() const;
    const Pen &GetPen() const;
    const Viewport &GetViewport();
    void SetUserShader(std::optional<Shader*> m_shader);
    std::optional<Shader*> GetUserShader() const;
    void Clear();
    ~Painter();
private:
    Position CalculateDrawPosition(const Position &originalDrawPos, const SizeF &originalDrawSize);
    UIControl *GetSender();
    UIControl *m_sender;
    std::unique_ptr<ClippingArea> m_clippingArea;
    std::optional<Shader*> m_userShader;
    Brush m_brush;
    Pen m_pen;
    float m_trasparencyFactor;
};

ARX_NAMESPACE_END

#endif
