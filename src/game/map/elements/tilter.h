#ifndef TILTER_H
#define TILTER_H

#include "../../../glm-include.h"
#include "../../sprites/UI/button.h"
#include "../../sprites/sprite.h"

#include <glmhelper.h>

//#define SEE_TILTER_MIRROR_POINTS

class Tilter : public Button
{
public:
  Tilter(Sprite base, glm::vec4 texOffset, Sprite mirror, glm::vec2 pivot, float initialAngle);
  void Update(glm::vec4 camRect, Input::Controls &input) { Update(camRect, input, 1.0f); }
  void Update(glm::vec4 camRect, Input::Controls &input, float scale) override;
  void Draw(Render *render) override;
  glm::vec4 getMirrorPoints();
  bool wasChanged() { return changed; }
  float getAngle() {  return  angle; }
  float getThickness() { return mirror.getTextureDim().y;  }
private:
  Sprite mirror;
  glm::vec2 pivot;
  float angle;
  glm::vec2 initialAngleVector;

  bool calculatedThisFrame = false;
  glm::vec4 mirrorPoints;
  
  bool selected = false;
  bool changed = false;
};


#endif
