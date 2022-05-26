#include "light_ray.h"

LightRay::LightRay(Resource::Texture pixel, glm::vec4 source, float angle, int staticLinesOffset, Sprite rayBox,  Sprite rayBoxOn, Sprite rayBoxOff)
{
  this->pixel = pixel;
  this->source = source;
  this->angle = angle;
  this->staticLinesOffset = staticLinesOffset;

  this->rayBoxOn = rayBoxOn;
  this->rayBoxOff = rayBoxOff;
  this->rayBox = rayBox;
  setSprites();
}

void LightRay::Update(std::vector<LightElements> &lightElems, glm::vec4 camRect)
{
  rayBox.Update(camRect);
  rayBoxOff.Update(camRect);
  rayBoxOn.Update(camRect);
  if(!on)
  {
    wasOff = true;
    rays.clear();
    return;
  }
  else
    calcPath(lightElems);
}

void LightRay::Draw(Render *render)
{
  //render->DrawQuad(pixel, glmhelper::calcMatFromRect(struc, 0.0f, 1.0f), glm::vec4(0.0f, 1.0f, 1.0f, 1.0f));

  rayBox.Draw(render);
  if(on)
    rayBoxOn.Draw(render);
  else
    rayBoxOff.Draw(render);
}

void LightRay::calcPath(std::vector<LightElements> &lightElems)
{
  rays.clear();

  const float STEP_SIZE = 1.0f;
  glm::vec2 deltaStep = glmhelper::getVectorFromAngle(angle) * STEP_SIZE;
  int reflections = 0;
  int steps = 0;

  glm::vec2 sourceVec = glm::vec2(this->source.x + this->source.z/2, this->source.y + this->source.w/2) + deltaStep*32.0f;
  glm::vec2 currentPos = sourceVec;
  float currentAngle = angle;

  while(true)
  {
    if(reflections > 10)
      break;
    currentPos += deltaStep;
    steps++;
    if(steps > 2000 / STEP_SIZE)
    {
      addRay(sourceVec, currentPos);
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
        addRay(sourceVec, currentPos);
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

void LightRay::setSprites()
{
  this->rayBox.setRect(source);
  //this->rayBox.setRotation(angle);
  this->rayBoxOn.setRect(source);
  this->rayBoxOn.setRotation(angle);
  this->rayBoxOff.setRect(source);
  this->rayBoxOff.setRotation(angle);
}


void LightRay::addRay(glm::vec2 sourceVec, glm::vec2 currentPos)
{
  DS::ShaderStructs::ray2D ray;
  ray.p1 = sourceVec;
  ray.p2 = currentPos;
  ray.magnitude = glm::distance(sourceVec, currentPos);
  rays.push_back(ray);
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
