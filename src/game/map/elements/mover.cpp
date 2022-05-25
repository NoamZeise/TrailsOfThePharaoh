#include "mover.h"


Mover::Mover(Sprite line, Sprite base, glm::vec4 rect, glm::vec2 lineStart, glm::vec2 lineEnd, int startIndex)
  : Button(base, false)
{
  this->lineSprite = line;
  this->unitLine = glm::normalize(lineEnd - lineStart);
  auto angle = glm::degrees(atan2(unitLine.y, unitLine.x));
  this->lineSprite.setModel(LightRay::GetLineTransform(lineStart, lineEnd, lineSprite.getTextureDim().y, angle));
  this->InitialRect = rect;
  this->lineIndex = startIndex;
  this->maxDistance = glm::distance(lineStart, lineEnd);
}

void Mover::Update(glm::vec4 camRect, Input::Controls &input, std::vector<LightRay::LightElements> &lightElems)
{
  if(firstUpdate)
  {
    sprite.setRect(InitialRect);
    firstUpdate = false;
    for(int i = lineIndex; i < lineIndex + 4; i++)
      originalLightElems.push_back(lightElems[i]);
  }

  Update(camRect, input, 1.0f);

  if(changedThisFrame)
  {
    auto changedLine = unitLine * currentDistance;
    sprite.setRect(glm::vec4(InitialRect.x + changedLine.x, InitialRect.y + changedLine.y, InitialRect.z, InitialRect.w));
    for(int i = lineIndex; i < lineIndex + 4; i++)
    {
      lightElems[i].p1 = originalLightElems[i - lineIndex].p1 + changedLine;
      lightElems[i].p2 = originalLightElems[i - lineIndex].p2 + changedLine;
      lightElems[i].changed = true;
    }
  }


  sprite.Update(camRect);

//check move

}

void Mover::Update(glm::vec4 camRect, Input::Controls &input, float scale)
{
  prevClicked = clicked;
  clicked = input.LeftMouse();
  glm::vec4 colour = glm::vec4(1.0f);
  changedThisFrame = false;

  if(gh::contains(input.MousePos(), sprite.getDrawRect()))
  {
    if(!clicked)
    {
      colour = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
    }
    else if(!prevClicked)
    {
      selected = true;
      prevMouse = input.MousePos();
    }
  }

  if(selected)
  {
    if(!clicked)
    {
      selected = false;
    }
    else
    {
      colour = glm::vec4(0.5f, 0.0f, 0.0f, 1.0f);

      auto mouseChanged = input.MousePos() - prevMouse;
      auto changeUnit = unitLine * mouseChanged;
      float changed = changeUnit.x + changeUnit.y;
      if(changed > 10)
        changed = 10;
      if(changed < -10)
        changed = -10;
      currentDistance += changed;
      currentDistance = currentDistance > maxDistance ? maxDistance : currentDistance < 0.0f ? 0.0f : currentDistance;

      changedThisFrame = changed != 0;

      prevMouse = input.MousePos();
    }
  }
  sprite.setColour(colour);

}

void Mover::Draw(Render *render)
{
  lineSprite.Draw(render);
  Button::Draw(render);
}
