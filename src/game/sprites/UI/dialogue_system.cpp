#include  "dialogue_system.h"


DialogueSystem::DialogueSystem(Sprite background, Sprite blurredBackground, Sprite messageBox, Resource::Font font)
{
  this->background  = background;
  this->blurredBackground = blurredBackground;
  this->messageBox = messageBox;
  this->font = font;
  this->initialBgRect = background.getDrawRect();
  this->inititalMsgRect = messageBox.getDrawRect();
}



void DialogueSystem::ShowMessage(std::string text, std::vector<Sprite> characters)
{
  messageSkipped = false;
  prevClicked = true;
  this->hasCharacter = true;
  this->characterSprite = characters;
  initialCharacterRect.clear();
  for(auto& ch :characters)
  {
    initialCharacterRect.push_back(ch.getDrawRect());
  }
  addText(text);
}

void DialogueSystem::ShowMessage(std::string text)
{
  currentText.clear();
  hasCharacter = false;
  messageSkipped = false;
  prevClicked = true;
  addText(text);
}

void DialogueSystem::addText(std::string text)
{
  currentText.clear();
  std::string nextLine = "";
  std::string lastWord = "";
  const int CHAR_WIDTH = 23.0f;
  const int BOX_WIDTH = 1000.0f;
  for(int i = 0; i < text.size(); i++)
  {
    lastWord += text[i];
    if(text[i] == ' ' || i == text.size() - 1)
    {
      if((nextLine.size() + lastWord.size() + 1) * CHAR_WIDTH > BOX_WIDTH)
      {
        this->currentText.push_back(nextLine);
        nextLine = lastWord;
        if(i != text.size() - 1)
          lastWord = "";
      }
      else
      {
        nextLine += lastWord;
        if(i == text.size() - 1)
          this->currentText.push_back(nextLine);
        lastWord = "";
      }
    }
  }
  if(lastWord.size() != 0)
    currentText.push_back(lastWord);
}

void DialogueSystem::Update(Timer &timer, Input::Controls &controls, glm::vec4 camRect, float scale)
{
  this->camRect = camRect;
  this->scale = scale;

  background.setRect(glmhelper::correctRectWithCamera(initialBgRect, camRect, scale));
  blurredBackground.setRect(glmhelper::correctRectWithCamera(initialBgRect, camRect, scale));
  messageBox.setRect(glmhelper::correctRectWithCamera(inititalMsgRect, camRect, scale));
  if(hasCharacter)
  {
    for(int i = 0; i < characterSprite.size(); i++)
    {
      characterSprite[i].setRect(glmhelper::correctRectWithCamera(initialCharacterRect[i], camRect, scale));
      characterSprite[i].Update(camRect);
    }
  }
  background.Update(camRect);
  blurredBackground.Update(camRect);
  messageBox.Update(camRect);

  bool clicked = controls.LeftMouse();
  if(clicked && !prevClicked)
    messageSkipped = true;
  prevClicked = clicked;
}

void DialogueSystem::Draw(Render *render)
{
  if(hasCharacter)
    blurredBackground.Draw(render);
  else
    background.Draw(render);
  messageBox.Draw(render);
  if(hasCharacter)
    for(auto &ch : characterSprite)
      ch.Draw(render);

  for(int i = 0; i < currentText.size(); i++)
  {
    render->DrawString(font, currentText[i], glm::vec2((420*scale) + camRect.x, (770 + i * 75)*scale + camRect.y), 50.0f*scale, 4.0f, glm::vec4(102.0f/255.0f,53.0f/255.0f,34.0f/255.0f, 1.0f));
  }
}
