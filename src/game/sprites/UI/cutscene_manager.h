#ifndef CUTSCENE_MANAGER_H
#define CUTSCENE_MANAGER_H

#include <string>
#include <fstream>
#include <vector>

#include "dialogue_system.h"
#include "../../../renderer-include.h"
#include <input.h>
#include <timer.h>

class CutsceneManager
{
public:
  CutsceneManager() {}
  CutsceneManager(Render* render, Resource::Font font);
  void Update(Timer &timer, Input::Controls &controls, glm::vec4 camRect, float camScale);
  void Draw(Render *render);
  void PlayCutscene(std::string cutsceneFile);
  bool isFinished() { return finished; }

private:

  DialogueSystem ds;

  enum class Character
  {
    None,
    Anubis,
    Mummy,
    Bod,
    EmptyBod,
    FullBod
  };
  struct Dialogue
  {
    Character character;
    std::string text;
  };

  std::vector<Dialogue> toRead;

  bool finished = true;

  Sprite Anubis;
  Sprite Mummy;
  Sprite Book;
  Sprite EmptyBook;
  Sprite FullBook;


};

#endif
