#include "mover.h"


Mover::Mover(Sprite line, Sprite outline, Sprite base, Sprite handle, glm::vec4 rect, glm::vec2 lineStart, glm::vec2 lineEnd, int startIndex, Audio::Manager *audio)
  : Button(base, false)
{
  this->audio = audio;

  this->lineSprite = line;
  this->unitLine = glm::normalize(lineEnd - lineStart);
  auto angle = glm::degrees(atan2(unitLine.y, unitLine.x));
  float length = gh::distance(lineStart, lineEnd);
  this->lineSprite.setModel(LightRay::GetLineTransform(lineStart, lineEnd, length * lineSprite.getTextureDim().y/lineSprite.getTextureDim().x, angle));
  this->InitialRect = rect;
  this->lineIndex = startIndex;
  this->maxDistance = glm::distance(lineStart, lineEnd);

  this->handle = handle;
  this->outline = outline;
}

void Mover::Update(glm::vec4 camRect, Input::Controls &input, std::vector<LightRay::LightElements> &lightElems)
{
  if(firstUpdate)
  {
    sprite.setRect(InitialRect);
    outline.setRect(
      glm::vec4(
        InitialRect.x - MOVER_OUTLINE_THICKNESS,
        InitialRect.y - MOVER_OUTLINE_THICKNESS,
        InitialRect.z + MOVER_OUTLINE_THICKNESS*2,
        InitialRect.w + MOVER_OUTLINE_THICKNESS*2
      )
    );
    handle.setRect(glm::vec4(InitialRect.x + InitialRect.z/2 - handle.getTextureDim().x/4, InitialRect.y + InitialRect.w/2 - handle.getTextureDim().y/4, handle.getTextureDim().x/2, handle.getTextureDim().y/2));
    firstUpdate = false;
    for(int i = lineIndex; i < lineIndex + 4; i++)
      originalLightElems.push_back(lightElems[i]);
  }

  Update(camRect, input, 1.0f);

  if(changedThisFrame)
  {
    auto changedLine = unitLine * currentDistance;
    sprite.setRect(glm::vec4(InitialRect.x + changedLine.x, InitialRect.y + changedLine.y, InitialRect.z, InitialRect.w));
    handle.setRect(glm::vec4(InitialRect.x + InitialRect.z/2 - handle.getTextureDim().x/4   + changedLine.x, InitialRect.y + InitialRect.w/2 - handle.getTextureDim().y/4 + changedLine.y, handle.getTextureDim().x/2, handle.getTextureDim().y/2));
        outline.setRect(
          glm::vec4(
            changedLine.x + InitialRect.x - MOVER_OUTLINE_THICKNESS,
            changedLine.y + InitialRect.y - MOVER_OUTLINE_THICKNESS,
            InitialRect.z + MOVER_OUTLINE_THICKNESS*2,
            InitialRect.w + MOVER_OUTLINE_THICKNESS*2
          )
        );
    for(int i = lineIndex; i < lineIndex + 4; i++)
    {
      lightElems[i].p1 = originalLightElems[i - lineIndex].p1 + changedLine;
      lightElems[i].p2 = originalLightElems[i - lineIndex].p2 + changedLine;
      lightElems[i].changed = true;
      lightElems[i].corrected = false;
    }
  }


  sprite.Update(camRect);
  outline.Update(camRect);
  handle.Update(camRect);

//check move

}

void Mover::Update(glm::vec4 camRect, Input::Controls &input, float scale)
{
  prevClicked = clicked;
  clicked = input.LeftMouse();
  glm::vec4 colour = glm::vec4(1.0f);
  changedThisFrame = false;
  beingControlled = false;
  hovering = false;

  if(gh::contains(input.MousePos(), sprite.getDrawRect()))
    colour = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);

  if(gh::contains(input.MousePos(), sprite.getDrawRect()))
  {
    if(!clicked)
    {
      hovering = true;
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
      if(!playingAudio)
      {
        playingAudio = true;
        if((int)InitialRect.x % 2 == 0)
          audio->Play("audio/sfx/Stone Slide1.wav", false, 0.15f);
        else
          audio->Play("audio/sfx/Stone Slide2.wav", false, 0.15f);
      }
      beingControlled = true;
      colour = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
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
  else
  {
    if(playingAudio)
    {
      playingAudio = false;
      if((int)InitialRect.x % 2 == 0)
        audio->Stop("audio/sfx/Stone Slide1.wav");
      else
        audio->Stop("audio/sfx/Stone Slide2.wav");
    }
  }
  sprite.setColour(colour);

}

void Mover::Draw(Render *render)
{
  lineSprite.Draw(render);
  Button::Draw(render);
  handle.Draw(render);
  if(beingControlled)
    outline.Draw(render);
}
