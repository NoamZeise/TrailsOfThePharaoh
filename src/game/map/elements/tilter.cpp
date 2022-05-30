#include "tilter.h"

Tilter::Tilter(Sprite base, Sprite outline, Sprite mirror, glm::vec2 pivot, float initialAngle, Audio::Manager *audio) : Button(base, false)
{
  this->audio = audio;

  this->mirror = mirror;
  this->outline = outline;
  this->pivot = pivot;

  this->initialAngleVector = glmhelper::getVectorFromAngle(initialAngle);

  this->angle = initialAngle;
  glm::vec2 dim = glm::vec2(base.getDrawRect().z*1.2f, TILTER_THICKNESS);
  this->mirror.setRect(glm::vec4(pivot.x - dim.x/2, pivot.y - dim.y/2, dim.x, dim.y));
  this->mirror.setRotation(initialAngle);
  sprite.setRotation(initialAngle);
  this->outline.setRect(sprite.getDrawRect());
  this->outline.setRotation(initialAngle);
}

  void Tilter::Update(glm::vec4 camRect, Input::Controls &input, float scale)
  {
    prevClicked = clicked;
    clicked = input.LeftMouse();
    changed = false;
    beingControlled = false;
    hovering = false;
    colour = glm::vec4(1.0f);

    if(gh::contains(input.MousePos(), sprite.getDrawRect()))
      colour = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);

    if(gh::contains(input.MousePos(), sprite.getDrawRect()) && !selected)
    {
      if(!clicked)
      {
        hovering = true;
        colour = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
      }
      else if(!prevClicked)
      {
        selected = true;
        prevMouseAngle = getMouseAngle(input);
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
          audio->Play("audio/sfx/Wheel spin1.wav", false, 0.15f);
        }

        beingControlled = true;
        auto mouseAngle = getMouseAngle(input);
        if(abs(mouseAngle - prevMouseAngle) > 330)
        {
          if(mouseAngle > prevMouseAngle)
            prevMouseAngle += 360.0f;
          else
            prevMouseAngle -= 360.0f;
        }

        changedAngle = (mouseAngle - prevMouseAngle) * 1.0f; //slow rotation
        prevMouseAngle = mouseAngle;
        offsetAngle(changedAngle);
      }
    }
    else
    {
      if(playingAudio)
      {
        playingAudio = false;
        audio->Stop("audio/sfx/Wheel spin1.wav");
      }
    }
    //sprite.setColour(colour);
    //mirror.setColour(colour);
    mirror.Update(camRect);
    sprite.Update(camRect);
    outline.Update(camRect);
  }

void Tilter::Draw(Render *render)
{
  #ifdef SEE_TILTER_MIRROR_POINTS
    auto pos = this->getMirrorPoints();
    render->DrawQuad(Resource::Texture(), glmhelper::getModelMatrix(glm::vec4(pos.x, pos.y, 10, 10), 0.0f, 5.0f));
    render->DrawQuad(Resource::Texture(), glmhelper::getModelMatrix(glm::vec4(pos.z, pos.w, 10, 10), 0.0f, 5.0f));
  #endif
  mirror.Draw(render);
  Button::Draw(render);
  if(beingControlled)
    outline.Draw(render);
}



glm::vec4 Tilter::getMirrorPoints()
{
  if(!calculatedThisFrame)
  {
    calculatedThisFrame = true;
    glm::vec2 angleVec  = glmhelper::getVectorFromAngle(angle);
    glm::vec2 dim = glm::vec2(mirror.getDrawRect().z, TILTER_THICKNESS);
    mirrorPoints =  glm::vec4(pivot.x + angleVec.x*dim.x/2, pivot.y + angleVec.y*dim.x/2,
                   pivot.x - angleVec.x*dim.x/2, pivot.y - angleVec.y*dim.x/2);
  }

  return mirrorPoints;
}

float Tilter::getMouseAngle(Input::Controls &controls)
{
  auto pos = glm::normalize(controls.MousePos() - pivot);
  return glm::degrees(atan2(pos.y, pos.x));
}
