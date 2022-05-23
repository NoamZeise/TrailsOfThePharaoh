#include "tilter.h"



  void Tilter::Update(glm::vec4 camRect, Input::Controls &input, float scale)
  {
    prevClicked = clicked;

    if(gh::contains(input.MousePos(), sprite.getDrawRect()) && !clicked)
    {
      clicked = input.LeftMouse();
      sprite.setColour(glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
      prevMouse = input.MousePos();
    }
    else if(!clicked)
    {
      clicked = false;
      sprite.setColour(glm::vec4(1.0f));
    }
    else
    {
      sprite.setColour(glm::vec4(0.8f, 0.8f, 0.2f, 1.0f));
      glm::vec2 mouseChange = prevMouse  - input.MousePos();

      //TODO: calc drag change and adjust mirror sprite




      prevMouse = input.MousePos();
    }
    mirror.Update(camRect);
    sprite.Update(camRect);
  }

  void Tilter::Draw(Render *render)
  {
    mirror.Draw(render);
    Button::Draw(render);
  }
