#ifndef LIGHT_RAY_H
#define LIGHT_RAY_H

#include <vector>

#include "../../../renderer-include.h"
#include "../../../glm-include.h"

#include "tilter.h"

#include <glmhelper.h>
#include <gamehelper.h>
#include <input.h>

class LightRay
{
public:

  struct LightElements
  {
    LightElements(glm::vec2 p1, glm::vec2 p2, float thickness, bool reflective);
    void Update(glm::vec2 p1, glm::vec2 p2);
    void correctNormal();
    glm::vec2 p1;
    glm::vec2 p2;
    float normal;
    float thickness;
    bool changed;
    bool reflective;
  };

  LightRay(Resource::Texture pixel, glm::vec4 source, float angle, int staticLinesOffset);
  void Update(std::vector<LightElements> &lightElems);
  void Draw(Render *render);

private:
  struct Ray
  {
    Ray(int p1I, int p2I) { this->p1Index = p1I; this->p2Index = p2I; }
    int p1Index = 0;
    int p2Index = 0;
  };
  void calcPath(std::vector<LightElements> &lightElems);
  void addRay(glm::vec2 sourceVec, glm::vec2 currentPos, float currentAngle, int p1I, int p2I);
  Resource::Texture pixel;
  glm::vec4 source;
  float angle;
  std::vector<glm::mat4> lightRayModels;
  std::vector<Ray> lightRayInfo;
  int staticLinesOffset = 0;
};


#endif
