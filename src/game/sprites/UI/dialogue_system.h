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
  DialogueSystem(Sprite background, Resource::Font font);
  void ShowMessage(std::string text);

  void Update(Timer &timer, Input::Controls &controls);

  bool showingMessage();
private:
  Sprite background;
  Resource::Font font;

};

#endif
