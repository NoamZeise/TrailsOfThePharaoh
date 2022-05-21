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
  Sprite(Resource::Texture texture, glm::vec4 drawRect);
  virtual void Update(glm::vec4 camRect);
  void Draw(Render *render);
  glm::vec4 getDrawRect() { return drawRect; }
  void setColour(glm::vec4 colour) { this->colour   = colour; }
  void setRect(glm::vec4 drawRect) { this->drawRect = drawRect; }

private:
  Resource::Texture texture;
  glm::vec4 drawRect;
  glm::vec4 colour = glm::vec4(1.0f);
  glm::mat4 model;
  bool toDraw = false;

};


#endif
