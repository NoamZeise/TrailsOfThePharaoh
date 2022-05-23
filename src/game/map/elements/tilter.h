#ifndef TILTER_H
#define TILTER_H

#include "../../sprites/UI/button.h"
#include "../../sprites/sprite.h"


class Tilter : public Button
{
public:
  Tilter(Sprite base, Sprite mirror) : Button(base, false)
  {
    this->mirror = mirror;
  }

  void Update(glm::vec4 camRect, Input::Controls &input, float scale) override;
  void Draw(Render *render) override;
private:
  Sprite mirror;
  glm::vec2 prevMouse;
  bool dragging = false;
};


#endif
