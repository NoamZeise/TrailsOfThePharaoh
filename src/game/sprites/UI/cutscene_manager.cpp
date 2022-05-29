#include "cutscene_manager.h"

  CutsceneManager::CutsceneManager(Render* render, Resource::Font font)
  {
    auto mummyTex = render->LoadTexture("textures/Story/Mummy Pose_1.png");
    Mummy = Sprite(
      mummyTex,
      glm::vec4(0.0f, -50.0f, mummyTex.dim.x/4, mummyTex.dim.y/4),
      3.4f
    );
    auto anubisTex = render->LoadTexture("textures/Story/Anubis.png");
    Anubis = Sprite(
      anubisTex,
      glm::vec4(1200.0f, -50.0f, anubisTex.dim.x/4, anubisTex.dim.y/4),
      3.4f
    );
    auto BODTex = render->LoadTexture("textures/Story/BookoftheDead_closed.png");
    Book = Sprite(
      BODTex,
      glm::vec4(400.0f, 0.0f, BODTex.dim.x/7, BODTex.dim.y/7),
      3.4f
    );
    auto textFieldTex = render->LoadTexture("textures/Story/textField.png");
    ds = DialogueSystem(
      Sprite(render->LoadTexture("textures/Story/Tomb_BG.png"), glm::vec4(0, 0, settings::TARGET_WIDTH, settings::TARGET_HEIGHT), 3.3f),
      Sprite(render->LoadTexture("textures/Story/Tomb_BG_blurred.png"), glm::vec4(0, 0, settings::TARGET_WIDTH, settings::TARGET_HEIGHT), 3.3f),
      Sprite(
        textFieldTex,
        glm::vec4((settings::TARGET_WIDTH - textFieldTex.dim.x*0.6f)*0.5f, 650, textFieldTex.dim.x*0.6, textFieldTex.dim.y*0.6),
        3.5f
      ),
      font
    );
  }

  void CutsceneManager::Update(Timer &timer, Input::Controls &controls, glm::vec4 camRect, float camScale)
  {
    if(!ds.showingMessage())
    {
      if(toRead.size() == 0)
        finished = true;
      else
      {
        auto next = toRead[0];
        toRead.erase(toRead.begin(), toRead.begin() + 1);

        switch(next.character)
        {
          case Character::Anubis:
            ds.ShowMessage(next.text, {Anubis});
            break;
          case Character::Mummy:
            ds.ShowMessage(next.text, {Mummy});
            break;
          case Character::Bod:
            ds.ShowMessage(next.text, {Anubis, Book});
            break;
          case Character::None:
            ds.ShowMessage(next.text);
            break;
        }

      }
    }

    ds.Update(timer, controls, camRect, camScale);
  }

  void CutsceneManager::Draw(Render *render)
  {
    ds.Draw(render);
  }

  void CutsceneManager::PlayCutscene(std::string cutsceneFile)
  {
    std::ifstream in(cutsceneFile);
    int i = 0;
    Dialogue nextDialogue;
    for(std::string line; std::getline(in, line);)
    {
      if(line.size() == 0)
        break;
      if(i % 2 == 0)
      {
        switch(line[0])
        {
          case 'A':
            nextDialogue.character = Character::Anubis;
            break;
          case 'P':
            nextDialogue.character = Character::Mummy;
            break;
          case 'B':
            nextDialogue.character = Character::Bod;
            break;
          default:
            nextDialogue.character = Character::None;
            break;
        }
      }
      else
      {
        nextDialogue.text = line;
        toRead.push_back(nextDialogue);
      }

      i++;
    }

    finished = false;
  }
