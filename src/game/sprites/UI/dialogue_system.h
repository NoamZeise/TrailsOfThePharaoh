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
  DialogueSystem(Sprite background, Sprite messageBox, Resource::Font font);
  void ShowMessage(std::string text, Sprite character, bool hasCharacter);
  void Update(Timer &timer, Input::Controls &controls, glm::vec4 camRect, float scale);
  void Draw(Render *render);

  bool showingMessage() { return !messageSkipped; }
private:
  Sprite background;
  Sprite messageBox;
  Resource::Font font;
  glm::vec4 initialBgRect;
  glm::vec4 inititalMsgRect;
  std::string currentText;
  bool hasCharacter;
  Sprite characterSprite;
  glm::vec4 initialCharacterRect;

  bool messageSkipped = false;
  bool prevClicked = true;
};

#endif
