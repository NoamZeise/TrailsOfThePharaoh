#ifndef DIALOGUE_SYSTEM_H
#define DIALOGUE_SYSTEM_H

#include <string>

#include "../sprite.h"
#include "../../../renderer-include.h"

#include <timer.h>
#include <input.h>



class DialogueSystem
{
public:
  DialogueSystem() {}
  DialogueSystem(Sprite background, Sprite blurredBackground, Sprite messageBox, Resource::Font font);
  void ShowMessage(std::string text, std::vector<Sprite> character);
  void ShowMessage(std::string text);
  void Update(Timer &timer, Input::Controls &controls, glm::vec4 camRect, float scale);
  void Draw(Render *render);

  bool showingMessage() { return !messageSkipped; }
private:

  void addText(std::string text);

  Sprite background;
  Sprite blurredBackground;
  Sprite messageBox;
  Resource::Font font;
  glm::vec4 initialBgRect;
  glm::vec4 inititalMsgRect;

  std::vector<std::string> currentText;

  bool hasCharacter;

  std::vector<Sprite> characterSprite;
  std::vector<glm::vec4> initialCharacterRect;

  bool messageSkipped = true;
  bool prevClicked = true;

  glm::vec4 camRect;
  float scale;
};

#endif
