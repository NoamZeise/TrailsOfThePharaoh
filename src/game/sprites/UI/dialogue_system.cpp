#include  "dialogue_system.h"


DialogueSystem::DialogueSystem(Sprite background, Sprite messageBox, Resource::Font font)
{
  this->background  = background;
  this->messageBox = messageBox;
  this->font = font;
  this->initialBgRect = background.getDrawRect();
  this->inititalMsgRect = messageBox.getDrawRect();
}



void DialogueSystem::ShowMessage(std::string text, Sprite character,  bool hasCharacter)
{
  messageSkipped = false;
  prevClicked = true;
  this->hasCharacter = hasCharacter;
  if(hasCharacter)
  {
    this->characterSprite.setRect(initialCharacterRect);
    this->characterSprite = character;
    this->initialCharacterRect = character.getDrawRect();
  }
  this->currentText = text;
}

void DialogueSystem::Update(Timer &timer, Input::Controls &controls, glm::vec4 camRect, float scale)
{
  background.setRect(glmhelper::correctRectWithCamera(initialBgRect, camRect, scale));
  messageBox.setRect(glmhelper::correctRectWithCamera(inititalMsgRect, camRect, scale));
  if(hasCharacter)
  {
    characterSprite.setRect(glmhelper::correctRectWithCamera(initialCharacterRect, camRect, scale));
    characterSprite.Update(camRect);
  }
  background.Update(camRect);
  messageBox.Update(camRect);

  bool clicked = controls.LeftMouse();
  if(clicked && !prevClicked)
    messageSkipped = true;
  prevClicked = clicked;
}

void DialogueSystem::Draw(Render *render)
{
  background.Draw(render);
  messageBox.Draw(render);
  if(hasCharacter)
    characterSprite.Draw(render);

  render->DrawString(font, currentText, glm::vec2(100, 800), 60.0f, 4.0f, glm::vec4(102.0f/255.0f,53.0f/255.0f,34.0f/255.0f, 1.0f));
}
