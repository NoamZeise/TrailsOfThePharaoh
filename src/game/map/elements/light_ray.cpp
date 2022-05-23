#include "light_ray.h"

LightRay::LightRay(Resource::Texture pixel, glm::vec4 source, float angle)
{
  this->pixel = pixel;
  this->source = source;
  this->angle = angle;
}

void LightRay::Update(std::vector<glm::vec4> &mirrors, std::vector<glm::vec4> &colliders, std::vector<Tilter> &tilters, glm::vec4 cameraRect)
{
  calcPath(mirrors, colliders, tilters);
}

void LightRay::Draw(Render *render)
{
  //render->DrawQuad(pixel, glmhelper::calcMatFromRect(struc, 0.0f, 1.0f), glm::vec4(0.0f, 1.0f, 1.0f, 1.0f));
  for(const auto& ray : lightRayModels)
  {
    render->DrawQuad(pixel, ray, glm::vec4(1.0f, 1.0f, 0.2f, 2.0f));
  }
}

void LightRay::calcPath(std::vector<glm::vec4> &mirrors, std::vector<glm::vec4> &colliders, std::vector<Tilter> &tilters)
{
  float thickness = 10.0f;
  lightRayModels.clear();
  glm::vec2 sourceVec = glm::vec2(this->source.x + this->source.z/2, this->source.y + this->source.w/2);
  glm::vec2 currentPos = sourceVec;
  float currentAngle = angle;

  const float STEP_SIZE = 1.0f;
  glm::vec2 deltaStep = glmhelper::getVectorFromAngle(currentAngle) * STEP_SIZE;
  int reflections = 0;
  int steps = 0;

  while(true)
  {
    if(reflections > 10)
      break;
    currentPos += deltaStep;
    steps++;
    if(steps > 2000 / STEP_SIZE)
    {
      addRay(sourceVec, currentPos, currentAngle);
      break;
    }
    bool struck = false;
    bool reflected = false;
    for(const auto &m: mirrors)
    {
      if(gh::contains(currentPos, m))
      {
      //            std::cout  <<  "mirror\n";
        addRay(sourceVec, currentPos, currentAngle);
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
        reflected = true;
        reflections++;
        steps = 0;
        break;
      }
    }
    for(auto &t: tilters)
    {
      auto mirrorPoints = t.getMirrorPoints();
      float normal = fmod(t.getAngle() + 90.0f, 360.0f);
      if(abs(normal - currentAngle) <= 90.0f || abs(normal  - currentAngle) >= 270.0f)
        normal = fmod(normal + 180.0f, 360.0f);
      auto p1 =  glm::vec2(mirrorPoints.x, mirrorPoints.y);
      auto p2 =  glm::vec2(mirrorPoints.z, mirrorPoints.w);
      auto correction = glmhelper::getVectorFromAngle(normal) * t.getThickness()/2.0f;
      p1 += correction;
      p2 += correction;
      if(gh::linesCross(sourceVec, currentPos, p1, p2))
      {
        addRay(sourceVec, currentPos, currentAngle);
        float incidence = (currentAngle) - normal;
        currentAngle -=  incidence*2;
        currentAngle += 180.0f;
        currentAngle = fmod(currentAngle, 360.0f);
        deltaStep = glmhelper::getVectorFromAngle(currentAngle) * STEP_SIZE;
        currentPos += deltaStep;
        sourceVec = currentPos;
        reflected = true;
        reflections++;
        steps = 0;
        break;
      }
    }
    if(!reflected)
    {
      for(const auto &c: colliders)
      {
        if(gh::contains(currentPos, c))
        {
          addRay(sourceVec, currentPos, currentAngle);
          struck = true;
          break;
        }
      }
    }
    if(struck)
      break;
  }
}

void LightRay::addRay(glm::vec2 sourceVec, glm::vec2 currentPos, float currentAngle)
{
  const float THICKNESS = 4.0f;
  glm::vec2 correction = glmhelper::getVectorFromAngle(currentAngle - 90.0f) * THICKNESS/2.0f;
  //std::cout << "correction: x:" << correction.x << "   y: " << correction.y << std::endl;
  lightRayModels.push_back(
    glmhelper::calcMatFromRect(
      glm::vec4(sourceVec.x + correction.x, sourceVec.y + correction.y, gh::distance(sourceVec, currentPos), THICKNESS),
      currentAngle, 1.0f, false));
}
