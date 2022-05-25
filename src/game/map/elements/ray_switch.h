#ifndef RAY_SWITCH_H
#define RAY_SWITCH_H

#include "light_switch.h"

class RaySwitch : public LightSwitch
{
public:
  RaySwitch(LightRay ray, bool on, int switchLineIndex, glm::vec4 rect, Sprite onSprite, Sprite offSprite)
    : LightSwitch(on, switchLineIndex, 4, rect, onSprite, offSprite)
  {
    this->lightRay = ray;
    this->lightRay.setOn(on);
  }

  void rayUpdate(std::vector<LightRay::LightElements> &lightElems, glm::vec4 camRect)
  {
    lightRay.Update(lightElems, camRect);
  }

  void Update(std::vector<LightRay::LightElements> &lightElems, glm::vec4 camRect) override
  {
    bool hit = false;
    for(int i = switchLineIndex; i < switchLineIndex + 4; i++)
      if(lightElems[i].lightHit)
      {
        for(int j = 0; j <lightElems[i].hitSource.size(); j++)
          if(!lightRay.hasRay(lightElems[i].hitSource[j], lightElems[i].hitDest[j]))
            hit = true;
      }

    if(hit)
      on = !naturalOn;
    else
      on = naturalOn;

    lightRay.setOn(on);

    UpdateSprites(camRect);
  }

  void Draw(Render *render) override
  {
    lightRay.Draw(render);
    LightSwitch::Draw(render);
  }

private:
  LightRay lightRay;
};


#endif
