#ifndef ARX_LABEL_H
#define ARX_LABEL_H
#include "arxengine/ui/UIControl.h"

ARX_NAMESPACE_BEGIN

class Label : public UIControl
{
public:
    Label(UIControl *parent, std::string_view text = "", Size size = constants::DEFAULT_SIZE, Position pos = constants::DEFAULT_POSITION);
    void SetText(std::string_view text); 
    const std::string &GetText() const;

    void SetTextColor(Color color);
    Color GetTextColor() const;

    void EnableFontSmoothing(bool enable);
private:
    void OnDraw(DrawEvent &e) override;
    std::string m_text;
    Color m_textColor;
};

ARX_NAMESPACE_END

#endif
