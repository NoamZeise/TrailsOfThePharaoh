#include "tilter.h"

Tilter::Tilter(Sprite base, Sprite mirror, glm::vec2 pivot, float initialAngle) : Button(base, false)
{
  this->mirror = mirror;
  this->pivot = pivot;

  this->initialAngleVector = glmhelper::getVectorFromAngle(initialAngle);

  this->angle = initialAngle;
  glm::vec2 dim = glm::vec2(base.getDrawRect().z, mirror.getTextureDim().y);
  this->mirror.setRect(glm::vec4(pivot.x - dim.x/2, pivot.y - dim.y/2, dim.x, dim.y));
  this->mirror.setRotation(initialAngle);
}

  void Tilter::Update(glm::vec4 camRect, Input::Controls &input, float scale)
  {
    prevClicked = clicked;
    clicked = input.LeftMouse();
    changed = false;
    beingControlled = false;
    hovering = false;
    colour = glm::vec4(1.0f);

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
        beingControlled = true;
        auto mouseAngle = getMouseAngle(input);
        if(abs(mouseAngle - prevMouseAngle) > 330)
        {
          if(mouseAngle > prevMouseAngle)
            prevMouseAngle += 360.0f;
          else
            prevMouseAngle -= 360.0f;
        }

        changedAngle = (mouseAngle - prevMouseAngle) * 0.15f; //slow rotation
        prevMouseAngle = mouseAngle;
        offsetAngle(changedAngle);
      }
    }
    sprite.setColour(colour);
    mirror.Update(camRect);
    sprite.Update(camRect);
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
}



glm::vec4 Tilter::getMirrorPoints()
{
  if(!calculatedThisFrame)
  {
    calculatedThisFrame = true;
    glm::vec2 angleVec  = glmhelper::getVectorFromAngle(angle);
    glm::vec2 dim = glm::vec2(sprite.getDrawRect().z, mirror.getTextureDim().y);
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
