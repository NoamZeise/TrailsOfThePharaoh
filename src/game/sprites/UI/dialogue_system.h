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
  DialogueSystem(Sprite background, Resource::Font font);
  void ShowMessage(std::string text, Sprite* character);
  void Update(Timer &timer, Input::Controls &controls, glm::vec4 camRect, float scale);
  void Draw(Render *render);

  bool showingMessage();
private:
  Sprite background;
  Resource::Font font;
  glm::vec4 initialBgRect;

  std::string currentText;
  Sprite* characterSprite = nullptr;
  glm::vec4 initialCharacterRect;
};

#endif
