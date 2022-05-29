#ifndef TILTER_H
#define TILTER_H

#include "../../../glm-include.h"
#include "../../sprites/UI/button.h"
#include "../../sprites/sprite.h"

#include <glmhelper.h>
#include <audio.h>

//#define SEE_TILTER_MIRROR_POINTS

const  float TILTER_THICKNESS =  50.0f;

class Tilter : public Button
{
public:
  Tilter(Sprite base, Sprite outline, Sprite mirror, glm::vec2 pivot, float initialAngle, Audio::Manager *audio);
  void Update(glm::vec4 camRect, Input::Controls &input) { Update(camRect, input, 1.0f); }
  void Update(glm::vec4 camRect, Input::Controls &input, float scale) override;
  void Draw(Render *render) override;
  glm::vec4 getMirrorPoints();
  bool wasChanged() { return changed; }
  float getAngle() {  return  angle; }
  float getThickness() { return TILTER_THICKNESS;  }
  float getChanged() {
    auto temp = changedAngle;
    changedAngle = 0.0f;
    return temp;  }
  void offsetAngle(float offset)
  {
    colour = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
    sprite.setColour(colour);
    angle += offset;
    changed = true;
    calculatedThisFrame = false;
    this->mirror.setRotation(angle);
    this->sprite.setRotation(angle);
    this->outline.setRotation(angle);
  }
  glm::vec4 getColour()
  {
    return colour;
  }
  void setColour(glm::vec4 colour)
  {
    this->colour = colour;
    sprite.setColour(colour);
  }

  bool isControlled() { return beingControlled; }
  bool isHovered() { return hovering; }
private:

  float getMouseAngle(Input::Controls &controls);

  Audio::Manager *audio;

  Sprite mirror;
  Sprite outline;

  glm::vec2 pivot;
  float angle;
  glm::vec2 initialAngleVector;

  float prevMouseAngle;

  bool calculatedThisFrame = false;
  glm::vec4 mirrorPoints;
  glm::vec4 colour  =  glm::vec4(1.0f);

  bool beingControlled = false;
  bool hovering = false;
  bool selected = false;
  bool changed = false;
  float changedAngle = 0.0f;

  bool playingAudio = false;

  bool first = true;
};


#endif
