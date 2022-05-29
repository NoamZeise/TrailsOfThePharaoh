#ifndef DOOR_SWITCH_H
#define DOOR_SWITCH_H

#include "light_switch.h"

class DoorSwitch : public LightSwitch
{
public:
  DoorSwitch(Sprite door, Sprite doorOff, glm::vec4 doorRect, int doorLineIndex, bool on, int switchLineIndex, glm::vec4 switchRect, Sprite onSprite, Sprite offSprite)
    : LightSwitch(on, switchLineIndex, 4, switchRect, onSprite, offSprite)
  {
    this->door = door;
    this->doorOff = doorOff;
    this->doorRect = doorRect;
    this->door.setRect(doorRect);
    this->doorOff.setRect(doorRect);
    this->doorOff.setColour(glm::vec4(1.0f, 1.0f, 1.0f, 0.5f));
    this->doorLineIndex = doorLineIndex;
  }

  void Update(std::vector<LightRay::LightElements> &lightElems, glm::vec4 camRect) override
  {
    if(firstUpdate)
    {
      firstUpdate = false;
      for(int i = doorLineIndex; i < doorLineIndex + 4; i++)
        onElements.push_back(lightElems[i]);
    }
    door.Update(camRect);
    doorOff.Update(camRect);
    for(int i = doorLineIndex; i < doorLineIndex + 4; i++)
    {
      if(!this->on)
        lightElems[i] = onElements[i - doorLineIndex];
      else
        lightElems[i] = offElem;
    }
    LightSwitch::Update(lightElems, camRect);
  }

  void Draw(Render* render) override
  {
    if(!this->on)
      door.Draw(render);
    else
      doorOff.Draw(render);
    LightSwitch::Draw(render);
  }

  glm::vec4 getDoorRect() { return door.getDrawRect(); }

private:
  Sprite door;
  Sprite doorOff;
  glm::vec4 doorRect;
  int doorLineIndex;
  bool firstUpdate = true;
  std::vector<LightRay::LightElements> onElements;
  LightRay::LightElements  offElem = LightRay::LightElements(glm::vec2(0), glm::vec2(0), 0.0f, false);
};

#endif
