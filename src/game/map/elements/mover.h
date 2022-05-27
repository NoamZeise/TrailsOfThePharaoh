#ifndef MOVER_H
#define MOVER_H

#include "light_ray.h"
#include "../../sprites/UI/button.h"

class Mover : public Button
{
public:
  Mover(Sprite line, Sprite base, Sprite handle, glm::vec4 rect, glm::vec2 lineStart, glm::vec2 lineEnd, int startIndex);
  void Update(glm::vec4 camRect, Input::Controls &input, std::vector<LightRay::LightElements> &lightElems);
  void Update(glm::vec4 camRect, Input::Controls &input, float scale) override;
  void Draw(Render *render) override;

  bool isMoving() { return beingControlled; }
  bool isHovered() { return hovering; }

private:
  Sprite lineSprite;
  Sprite  handle;
  glm::vec4 InitialRect;

  bool beingControlled = false;
  bool hovering;
  bool selected = false;
  glm::vec2 prevMouse;

  glm::vec2 unitLine;
  float maxDistance;
  float currentDistance = 0.0f;

  bool firstUpdate = true;
  bool changedThisFrame = false;
  int lineIndex;
  std::vector<LightRay::LightElements> originalLightElems;
};

#endif
