#ifndef LIGHT_RAY_H
#define LIGHT_RAY_H

#include <vector>

#include <glmhelper.h>
#include <gamehelper.h>
#include "../../../glm-include.h"
#include "../../../renderer-include.h"

class LightRay
{
public:
  LightRay(Resource::Texture pixel, glm::vec4 source, float angle);
  void Update(std::vector<glm::vec4> &mirrors, glm::vec4 cameraRect);
  void Draw(Render *render);

private:
  Resource::Texture pixel;
  glm::vec4 source;
  float angle;
  std::vector<glm::mat4> lightRayModels;
};


#endif
