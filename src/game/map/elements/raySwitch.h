#ifndef RAY_SWITCH_H
#define RAY_SWITCH_H

#include "light_ray.h"

class RaySwitch
{
public:
  RaySwitch(LightRay ray, glm::vec4 rect, bool on, std::vector<LightRay::LightElements*> switchLines)
  {
    this->lightRay = ray;
    this->naturalOn = on;
    this->lightRay.setOn(on);
    this->switchLines = switchLines;
  }

  void Update(std::vector<LightRay::LightElements> &lightElems)
  {
    lightRay.Update(lightElems);
    bool hit = false;
    for(auto &line: switchLines)
      if(line->lightHit)
      {
        for(int i = 0;  i  < line->hitSource.size(); i++)
          if(!lightRay.hasRay(line->hitSource[i], line->hitDest[i]))
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
  std::vector<LightRay::LightElements*> switchLines;
};


#endif
