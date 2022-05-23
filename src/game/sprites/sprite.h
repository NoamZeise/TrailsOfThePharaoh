#ifndef SPRITE_H
#define SPRITE_H

#include <timer.h>
#include <gamehelper.h>
#include <glmhelper.h>
#include "../../renderer-include.h"
#include "../../glm-include.h"

class Sprite
{
public:
  Sprite() {}
  Sprite(Resource::Texture texture, glm::vec4 drawRect, float depth);
  virtual void Update(glm::vec4 camRect);
  void Draw(Render *render);
  glm::vec4 getDrawRect() { return drawRect; }
  void setColour(glm::vec4 colour) { this->colour   = colour; }
  void setRect(glm::vec4 drawRect)
  {
    this->drawRect = drawRect;
    changed = true;
  }
  void setRotation(float rotation)
  {
    this->rotation = rotation;
    changed = true;
  }

private:
  Resource::Texture texture;
  glm::vec4 drawRect;
  float rotation = 0.0f;
  float depth = 0.0f;
  glm::vec4 colour = glm::vec4(1.0f);
  glm::mat4 model;
  bool toDraw = false;
  bool changed = true;
};


#endif
