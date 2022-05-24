#ifndef RAY_SWITCH_H
#define RAY_SWITCH_H

#include "light_ray.h"

class RaySwitch
{
public:
  RaySwitch(LightRay ray, glm::vec4 rect, bool on, int switchLineIndex)
  {
    this->lightRay = ray;
    this->naturalOn = on;
    this->lightRay.setOn(on);
    this->switchLineIndex = switchLineIndex;
  }

  void Update(std::vector<LightRay::LightElements> &lightElems)
  {
    lightRay.Update(lightElems);
    bool hit = false;
    for(int i = switchLineIndex; i < switchLineIndex + 4; i++)
      if(lightElems[i].lightHit)
      {
        for(int j = 0;  j  < lightElems[i].hitSource.size(); j++)
          if(!lightRay.hasRay(lightElems[i].hitSource[j], lightElems[i].hitDest[j]))
            hit = true;
      }

    if(hit)
      lightRay.setOn(!naturalOn);
    else
      lightRay.setOn(naturalOn);
  }

  void Draw(Render *render)
  {
    lightRay.Draw(render);
  }

private:
  LightRay lightRay;
  bool naturalOn;
  int switchLineIndex;
};


#endif
