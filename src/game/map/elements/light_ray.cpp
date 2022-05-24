#include "light_ray.h"

LightRay::LightRay(Resource::Texture pixel, glm::vec4 source, float angle, int staticLinesOffset)
{
  this->pixel = pixel;
  this->source = source;
  this->angle = angle;
  this->staticLinesOffset = staticLinesOffset;
}

void LightRay::Update(std::vector<LightElements> &lightElems)
{
  if(!on)
  {
    wasOff = true;
    lightRayModels.clear();
    lightRayInfo.clear();
    return;
  }
  else
    calcPath(lightElems);
}

void LightRay::Draw(Render *render)
{
  //render->DrawQuad(pixel, glmhelper::calcMatFromRect(struc, 0.0f, 1.0f), glm::vec4(0.0f, 1.0f, 1.0f, 1.0f));
  for(const auto& ray : lightRayModels)
  {
    render->DrawQuad(pixel, ray, glm::vec4(1.0f, 1.0f, 0.2f, 2.0f));
  }
}

void LightRay::calcPath(std::vector<LightElements> &lightElems)
{
  lightRayModels.clear();
  lightRayInfo.clear();
  glm::vec2 sourceVec = glm::vec2(this->source.x + this->source.z/2, this->source.y + this->source.w/2);
  glm::vec2 currentPos = sourceVec;
  float currentAngle = angle;

  const float STEP_SIZE = 1.0f;
  glm::vec2 deltaStep = glmhelper::getVectorFromAngle(currentAngle) * STEP_SIZE;
  int reflections = 0;
  int steps = 0;
  int  prevIndex = -1;

  while(true)
  {
    if(reflections > 50)
      break;
    currentPos += deltaStep;
    steps++;
    if(steps > 2000 / STEP_SIZE)
    {
      addRay(sourceVec, currentPos, currentAngle, prevIndex,  -1);
      break;
    }
    bool struck = false;

    for(int i = 0; i < lightElems.size();  i++)
    {
      lightElems[i].changed = false;
      auto normal = lightElems[i].normal;
      if(abs(normal - currentAngle) <= 90.0f || abs(normal  - currentAngle) >= 270.0f)
        normal = fmod(normal + 180.0f, 360.0f);

      auto correction = glmhelper::getVectorFromAngle(normal) * lightElems[i].thickness/2.0f;

      glm::vec2 p1 = lightElems[i].p1 + correction;
      glm::vec2 p2 = lightElems[i].p2 + correction;
      if(gh::linesCross(sourceVec, currentPos, p1, p2))
      {
        lightElems[i].lightHit = true;
        lightElems[i].hitSource.push_back(sourceVec);
        lightElems[i].hitDest.push_back(currentPos);
        addRay(sourceVec, currentPos, currentAngle, prevIndex, i);
        prevIndex = i;
        if(lightElems[i].reflective)
        {
          float incidence = (currentAngle) - normal;
          currentAngle -=  incidence * 2.0f;
          currentAngle += 180.0f;
          currentAngle = fmod(currentAngle, 360.0f);
          deltaStep = glmhelper::getVectorFromAngle(currentAngle) * STEP_SIZE;
          currentPos += deltaStep;
          sourceVec = currentPos;
          reflections++;
          steps = 0;
        }
        else
        {
          struck = true;
        }
      }
    }
    if(struck)
      break;
  }
}



void LightRay::addRay(glm::vec2 sourceVec, glm::vec2 currentPos, float currentAngle, int p1I, int p2I)
{
  lightRayModels.push_back(GetLineTransform(sourceVec, currentPos, 4.0f, currentAngle));
  lightRayInfo.push_back(Ray(p1I, p2I, sourceVec, currentPos));
}


LightRay::LightElements::LightElements(glm::vec2 p1, glm::vec2 p2, float thickness, bool reflective)
{
  this->p1 = p1;
  this->p2 = p2;
  this->thickness = thickness;
  this->changed = true;
  this->reflective = reflective;
  correctNormal();
  const float EXTRA_MARGIN = 1.0f;
  auto angle = glm::radians(normal - 90.0f);
  glm::vec2 unitVec = glm::vec2(cos(angle), sin(angle));
  this->p1 += unitVec * EXTRA_MARGIN;
  this->p2 -= unitVec * EXTRA_MARGIN;
}
void LightRay::LightElements::Update(glm::vec2 p1, glm::vec2 p2)
{
  changed = p1 != this->p1 || p2 != this->p2;
  this->p1 = p1;
  this->p2 = p2;
  if(changed)
    correctNormal();
}

void LightRay::LightElements::correctNormal()
{
  this->normal = fmod(glm::degrees(atan2(p1.y - p2.y, p1.x - p2.x)) + 90.0f, 360.0f);
}
