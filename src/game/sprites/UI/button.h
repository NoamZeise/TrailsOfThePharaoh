#ifndef BUTTON_H
#define BUTTON_H

#include "../sprite.h"
#include "../../../glm-include.h"

#include <input.h>

class Button
{
public:
  Button() {}
  Button(Sprite sprite, bool isStatic) { this->sprite = sprite; this->initialRect = sprite.getDrawRect(); this->isStatic = isStatic; }
  void Update(glm::vec4 camRect, Input::Controls &input, float scale);
  void Draw(Render *render) { sprite.Draw(render); }
  bool Clicked()  { return !prevClicked && clicked; }

private:
  Sprite sprite;
  bool prevClicked = false;
  bool clicked = false;
  bool isStatic;
  glm::vec4 initialRect;
};

#endif
