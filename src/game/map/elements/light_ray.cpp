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

  const int START_STEP_SIZE = 50.0f;
  float stepSize = START_STEP_SIZE;
  glm::vec2 unitStep = glmhelper::getVectorFromAngle(angle);
  int reflections = 0;
  int steps = 0;

  glm::vec2 sourceVec = glm::vec2(this->source.x + this->source.z/2, this->source.y + this->source.w/2) + unitStep*32.0f;
  glm::vec2 currentPos = sourceVec;
  float currentAngle = angle;

  std::vector<LightElements*> crossedElems;
  while(true)
  {
    if(reflections > 10)
      break;
    currentPos += unitStep*stepSize;
    steps++;
    if(steps > 100)
    {
      addRay(sourceVec, currentPos);
      break;
    }
    bool struck = false;
    crossedElems.clear();

    for(int i = 0; i < lightElems.size();  i++)
    {
      lightElems[i].changed = false;
    //  if(!lightElems[i].corrected)
    //  {
        lightElems[i].corrected = true;
        lightElems[i].correctedNormal = lightElems[i].normal;
        if(abs(lightElems[i].correctedNormal - currentAngle) <= 90.0f || abs(lightElems[i].correctedNormal  - currentAngle) >= 270.0f)
          lightElems[i].correctedNormal = fmod(lightElems[i].correctedNormal + 180.0f, 360.0f);

        auto correction = glmhelper::getVectorFromAngle(lightElems[i].correctedNormal) * lightElems[i].thickness/2.0f;

        lightElems[i].p1Corrected = lightElems[i].p1 + correction;
        lightElems[i].p2Corrected = lightElems[i].p2 + correction;
      //}
      if(gh::linesCross(sourceVec, currentPos, lightElems[i].p1Corrected, lightElems[i].p2Corrected))
      {
        crossedElems.push_back(&lightElems[i]);
      }
    }

    if(crossedElems.size() > 0)
    {
      LightElements* closestCrossed = nullptr;
      float closestDist = -1.0f;
      for(auto &elem: crossedElems)
      {
        float dist = -1.0f;
        elem->lightHit = true;
        elem->hitSource.push_back(sourceVec);
        elem->hitDest.push_back(currentPos);
        glm::vec2 lineVec = elem->p2Corrected - elem->p1Corrected;
        float l2 = (lineVec.x* lineVec.x) + (lineVec.y*lineVec.y);
        if(l2 == 0.0f)
          dist = glm::distance(elem->p1Corrected, sourceVec);
        else
        {
          float t = fmax(0, fmin(1, glm::dot(sourceVec - elem->p1Corrected, elem->p2Corrected - elem->p1Corrected) / l2));
          glm::vec2 projOntoElem = elem->p1Corrected + (elem->p2Corrected - elem->p1Corrected)*t;
          dist = glm::distance(sourceVec, projOntoElem);
        }
        if(closestDist == -1.0f || dist < closestDist)
        {
          closestCrossed = elem;
          closestDist = dist;
        }
      }
        float dir = -1.0f;
        while(stepSize > 1.0f)
        {
          currentPos += unitStep*stepSize*dir;
          stepSize /= 2.0f;
          if(gh::linesCross(sourceVec, currentPos, closestCrossed->p1Corrected, closestCrossed->p2Corrected))
          {
            dir = -1.0f;
          }
          else
          {
            dir = 1.0f;
          }
        }

        closestCrossed->lightHit = true;
        closestCrossed->hitSource.push_back(sourceVec);
        closestCrossed->hitDest.push_back(currentPos);
        addRay(sourceVec, currentPos);
        if(closestCrossed->reflective)
        {
          float incidence = (currentAngle) - closestCrossed->correctedNormal;
          currentAngle -=  incidence * 2.0f;
          currentAngle += 180.0f;
          currentAngle = fmod(currentAngle, 360.0f);
          unitStep = glmhelper::getVectorFromAngle(currentAngle);
          currentPos += unitStep*3.0f;
          stepSize = START_STEP_SIZE;
          sourceVec = currentPos;
          reflections++;
          steps = 0;

        }
        else
        {
          struck = true;
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
