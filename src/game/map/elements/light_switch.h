#ifndef LIGHT_SWITCH_H
#define LIGHT_SWITCH_H

#include "light_ray.h"

class LightSwitch
{
public:
  LightSwitch() {}
  LightSwitch(bool on, int switchLineIndex, int lineCount, glm::vec4 rect, Sprite onSprite, Sprite offSprite)
  {
    this->on = on;
    this->naturalOn = on;
    this->switchLineIndex = switchLineIndex;
    this->lineCount = lineCount;

    if(on)
    {
      this->offSprite = onSprite;
      this->onSprite = offSprite;
    }
    else
    {
      this->onSprite = onSprite;
      this->offSprite = offSprite;
    }
    this->onSprite.setRect(rect);
    this->offSprite.setRect(rect);
    this->rect = onSprite.getDrawRect();
  }

  virtual void Update(std::vector<LightRay::LightElements> &lightElems, glm::vec4 camRect)
  {
    UpdateSprites(camRect);
    on = naturalOn;
    for(int i = switchLineIndex; i < switchLineIndex + lineCount; i++)
      if(lightElems[i].lightHit)
        on = !naturalOn;
  }

  virtual void Draw(Render* render)
  {
    if(on)
      onSprite.Draw(render);
    else
      offSprite.Draw(render);
  }

  bool isOn() { return on; }

protected:

  void UpdateSprites( glm::vec4 camRect)
  {
    onSprite.Update(camRect);
    offSprite.Update(camRect);
  }

  bool on;
  bool naturalOn;
  int switchLineIndex;
  int lineCount;

  glm::vec4 rect;
  Sprite onSprite;
  Sprite offSprite;
};

#endif
