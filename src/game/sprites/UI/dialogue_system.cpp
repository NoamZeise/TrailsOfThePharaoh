#include  "dialogue_system.h"


DialogueSystem::DialogueSystem(Sprite background, Resource::Font font)
{
  this->background  = background;
  this->font = font;
  this->initialBgRect = background.getDrawRect();
}



void DialogueSystem::ShowMessage(std::string text, Sprite* character)
{
  if(this->characterSprite != nullptr)
    this->characterSprite->setRect(initialCharacterRect);
  this->characterSprite = character;
  this->initialCharacterRect = character->getDrawRect();
  this->currentText = text;
}

void DialogueSystem::Update(Timer &timer, Input::Controls &controls, glm::vec4 camRect, float scale)
{
  background.setRect(glmhelper::correctRectWithCamera(initialBgRect, camRect, scale));
  if(characterSprite)
  {
    characterSprite->setRect(glmhelper::correctRectWithCamera(initialCharacterRect, camRect, scale));
    characterSprite->Update(camRect);
  }

  background.Update(camRect);
}

void DialogueSystem::Draw(Render *render)
{
  background.Draw(render);
  if(characterSprite)
    characterSprite->Draw(render);

  render->DrawString(font, currentText, glm::vec2(100, 800), 60.0f, 4.0f, glm::vec4(102.0f/255.0f,53.0f/255.0f,34.0f/255.0f, 1.0f));
}
