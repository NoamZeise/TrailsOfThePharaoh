#ifndef LIGHT_SWITCH_H
#define LIGHT_SWITCH_H

#include "light_ray.h"

class LightSwitch
{
public:
  LightSwitch() {}
  LightSwitch(bool on, int switchLineIndex, int lineCount)
  {
    this->on = on;
    this->naturalOn = on;
    this->switchLineIndex = switchLineIndex;
    this->lineCount = lineCount;
  }

  void Update(std::vector<LightRay::LightElements> &lightElems)
  {
    on = naturalOn;
    for(int i = switchLineIndex; i < switchLineIndex + lineCount; i++)
      if(lightElems[i].lightHit)
        on = !naturalOn;
  }

  bool isOn() { return on; }

private:
  bool on;
  bool naturalOn;
  int switchLineIndex;
  int lineCount;
};

#endif
