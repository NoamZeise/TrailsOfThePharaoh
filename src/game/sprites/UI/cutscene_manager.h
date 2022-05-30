#ifndef CUTSCENE_MANAGER_H
#define CUTSCENE_MANAGER_H

#include <string>
#include <fstream>
#include <vector>

#include "dialogue_system.h"
#include "../../../renderer-include.h"
#include <input.h>
#include <timer.h>
#include <audio.h>

class CutsceneManager
{
public:
  CutsceneManager() {}
  CutsceneManager(Render* render, Resource::Font font, Audio::Manager *audio);
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
    FullBod,
    Title,
    Credit
  };
  struct Dialogue
  {
    Character character;
    std::string text;
    std::string audioPath;
  };

  Audio::Manager *audio;
  std::string lastAudio;

  std::vector<Dialogue> toRead;

  bool finished = true;
  bool showingTitle = false;
  bool showingEnd = false;

  Sprite Anubis;
  Sprite Mummy;
  Sprite Book;
  Sprite EmptyBook;
  Sprite FullBook;

  Sprite Background;
  Sprite EmptyBookTitle;
  Resource::Font font;
};

#endif
