#include "button.h"

void Button::Update(glm::vec4 camRect, Input::Controls &input, float scale)
{
  prevClicked = clicked;
  clicked = input.LeftMouse();
  if(gh::contains(input.MousePos(), sprite.getDrawRect()))
  {
    onSprite = true;
    sprite.setColour(glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
  }
  else
  {
    onSprite = false;
    sprite.setColour(glm::vec4(1.0f));
  }

  if(isStatic)
    sprite.setRect(glm::vec4((initialRect.x*scale)+camRect.x, (initialRect.y*scale)+camRect.y, initialRect.z*scale, initialRect.w*scale));

  sprite.Update(camRect);
}
