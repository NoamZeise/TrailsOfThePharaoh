#include "light_ray.h"

LightRay::LightRay(Resource::Texture pixel, glm::vec4 source, float angle)
{
  this->pixel = pixel;
  this->source = source;
  this->angle = angle;
}

void LightRay::Update(std::vector<glm::vec4> &mirrors, std::vector<glm::vec4> &colliders, glm::vec4 cameraRect, Input::Controls &controls)
{
  if(controls.Up())
    angle += 0.05f;
  if(controls.Down())
    angle -= 0.05f;
  calcPath(mirrors, colliders);
}

void LightRay::Draw(Render *render)
{
  //render->DrawQuad(pixel, glmhelper::calcMatFromRect(struc, 0.0f, 1.0f), glm::vec4(0.0f, 1.0f, 1.0f, 1.0f));
  for(const auto& ray : lightRayModels)
  {
    render->DrawQuad(pixel, ray, glm::vec4(1.0f, 1.0f, 0.2f, 2.0f));
  }
}

void LightRay::calcPath(std::vector<glm::vec4> &mirrors, std::vector<glm::vec4> &colliders)
{
  float thickness = 4.0f;
  lightRayModels.clear();
  glm::vec2 sourceVec = glm::vec2(this->source.x + this->source.z/2, this->source.y + this->source.w/2);
  glm::vec2 currentPos = sourceVec;
  float currentAngle = angle;
  glm::vec2 deltaStep = glm::vec2(cos(glm::radians(currentAngle)), sin(glm::radians(currentAngle))) / 2.0f;
  int reflections = 0;
  while(true)
  {
    if(reflections > 10)
      break;
    currentPos += deltaStep;
    float dist = gh::distance(sourceVec, currentPos);
    if(dist > 1000)
    {
      lightRayModels.push_back(glmhelper::calcMatFromRect(glm::vec4(sourceVec, dist, thickness), currentAngle, 1.0f, false));
      break;
    }
    bool struck = false;
    bool reflected = false;
    for(const auto &m: mirrors)
    {
      if(gh::contains(currentPos, m))
      {
        lightRayModels.push_back(glmhelper::calcMatFromRect(glm::vec4(sourceVec, dist, thickness), currentAngle, 1.0f, false));
        if(gh::contains(glm::vec2(currentPos.x - deltaStep.x, currentPos.y), m))//flat hit
        {
          deltaStep.y *= -1;
        }
        else
        {
          deltaStep.x *= -1;
        }
        currentAngle =  glm::degrees(atan2(deltaStep.y, deltaStep.x));
        sourceVec = currentPos;
        currentPos += deltaStep;
        reflected = true;
        reflections++;
        break;
      }
    }
    if(!reflected)
    {
      for(const auto &c: colliders)
      {

        if(gh::contains(currentPos, c))
        {
          lightRayModels.push_back(glmhelper::calcMatFromRect(glm::vec4(sourceVec, dist, thickness), currentAngle, 1.0f, false));
          struck = true;
          break;
        }
      }
    }
    if(struck)
      break;
  }
}
