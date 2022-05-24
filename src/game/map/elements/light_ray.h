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
    bool isTarget = false;
    bool lightHit = false;
    std::vector<glm::vec2> hitSource;
    std::vector<glm::vec2> hitDest;
  };
  LightRay()  {}
  LightRay(Resource::Texture pixel, glm::vec4 source, float angle, int staticLinesOffset);
  void Update(std::vector<LightElements> &lightElems);
  void Draw(Render *render);
  void setOn(bool on)
  {
    this->on = on;
  }
  bool hasRay(glm::vec2 source, glm::vec2 dest)
  {
    for(const auto& ray: lightRayInfo)
    {
      if(ray.source == source && ray.dest == dest)
        return true;
    }
    return false;
  }

  static glm::mat4 GetLineTransform(glm::vec2 p1, glm::vec2 p2, float thickness, float angle)
  {
    glm::vec2 correction = glmhelper::getVectorFromAngle(angle - 90.0f) * thickness/2.0f;
    return glmhelper::calcMatFromRect(
      glm::vec4(p1.x + correction.x, p1.y + correction.y, gh::distance(p1, p2), thickness),
      angle, 0.9f, false);
  }

private:
  struct Ray
  {
    Ray(int p1I, int p2I, glm::vec2 source, glm::vec2 dest) { this->p1Index = p1I; this->p2Index = p2I; this->source = source; this->dest = dest; }
    int p1Index = 0;
    int p2Index = 0;
    glm::vec2 source;
    glm::vec2 dest;
  };
  void calcPath(std::vector<LightElements> &lightElems);
  void addRay(glm::vec2 sourceVec, glm::vec2 currentPos, float currentAngle, int p1I, int p2I);
  Resource::Texture pixel;
  glm::vec4 source;
  float angle;
  std::vector<glm::mat4> lightRayModels;
  std::vector<Ray> lightRayInfo;
  int staticLinesOffset = 0;
  bool on = true;
  bool wasOff = false;
};


#endif
