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
  LightRay(Resource::Texture pixel, glm::vec4 source, float angle);
  void Update(std::vector<glm::vec4> &mirrors, std::vector<glm::vec4> &colliders, std::vector<Tilter> &tilters, glm::vec4 cameraRect);
  void Draw(Render *render);

private:

  void calcPath(std::vector<glm::vec4> &mirrors, std::vector<glm::vec4> &colliders, std::vector<Tilter> &tilters);
  void addRay(glm::vec2 sourceVec, glm::vec2 currentPos, float currentAngle);
  Resource::Texture pixel;
  glm::vec4 source;
  float angle;
  std::vector<glm::mat4> lightRayModels;
};


#endif
