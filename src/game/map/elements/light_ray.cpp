#include "light_ray.h"

LightRay::LightRay(Resource::Texture pixel, glm::vec4 source, float angle)
{
  this->pixel = pixel;
  this->source = source;
  this->angle = angle;
}

void LightRay::Update(std::vector<glm::vec4> &mirrors, glm::vec4 cameraRect)
{
  lightRayModels.clear();
  lightRayModels.push_back(glmhelper::calcMatFromRect(glm::vec4(10, 10, 100, 10), 1.0f));
}

void LightRay::Draw(Render *render)
{
  for(const auto& ray : lightRayModels)
  {
    render->DrawQuad(pixel, ray, glm::vec4(1.0f, 1.0f, 0.2f, 1.0f));
  }
}
