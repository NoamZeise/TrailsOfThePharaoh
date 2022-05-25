#ifndef TEXT_BUTTON_H
#define TEXT_BUTTON_H

#include <string>

#include "button.h"
#include "../../../renderer-include.h"


class TextButton : public Button
{
public:
  TextButton() {}
  TextButton(Sprite background, bool isStatic, std::string text, Resource::Font font)
    : Button(background, isStatic)
  {
    this->text = text;
    this->font = font;
    this->depth = background.getDepth() + 0.05f;
    setTextProps(background.getDrawRect());
  }
  void Update(glm::vec4 camRect, Input::Controls &input, float scale) override
  {
    Button::Update(camRect, input, scale);
    if(isStatic)
      setTextProps(sprite.getDrawRect());
  }
  void Draw(Render* render) override
  {
    Button::Draw(render);
    render->DrawString(font, text, pos, size, depth, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
  }

private:
  void setTextProps(glm::vec4 bgRect)
  {
    this->size = bgRect.z / 3.0f;
    this->pos = glm::vec2(
      bgRect.x + bgRect.z/10.0f,
      bgRect.y + bgRect.w/1.5f
    );
  }

  std::string text;
  Resource::Font font;
  glm::vec2 pos;
  float size;
  float depth;
};

#endif
