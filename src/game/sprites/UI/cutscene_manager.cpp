#include "cutscene_manager.h"

  CutsceneManager::CutsceneManager(Render* render, Resource::Font font, Audio::Manager *audio)
  {
    this->audio = audio;
    this->font = font;
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
    auto EmptyBODTex = render->LoadTexture("textures/Story/BookoftheDead_open_empty.png");
    EmptyBook = Sprite(
      EmptyBODTex,
      glm::vec4(200.0f, 0.0f, EmptyBODTex.dim.x/7, EmptyBODTex.dim.y/7),
      3.4f
    );
    EmptyBookTitle = Sprite(
      render->LoadTexture("textures/Story/BookTitle.png"),
      glm::vec4(200.0f, 0.0f, EmptyBODTex.dim.x/7, EmptyBODTex.dim.y/7),
      3.4f
    );
    auto FullBODTex = render->LoadTexture("textures/Story/BookoftheDead_open.png");
    FullBook = Sprite(
      FullBODTex,
      glm::vec4(200.0f, 0.0f, FullBODTex.dim.x/7, FullBODTex.dim.y/7),
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

    this->Background = Sprite(render->LoadTexture("textures/Story/Tomb_BG.png"), glm::vec4(0, 0, settings::TARGET_WIDTH, settings::TARGET_HEIGHT), 3.3f);
  }

  void CutsceneManager::Update(Timer &timer, Input::Controls &controls, glm::vec4 camRect, float camScale)
  {
    if(!ds.showingMessage())
    {
      if(toRead.size() == 0)
        finished = true;
      else
      {
        showingTitle = false;
        showingEnd = false;
        auto next = toRead[0];
        toRead.erase(toRead.begin(), toRead.begin() + 1);
        if(lastAudio != "")
          audio->Stop(lastAudio);
        if(next.audioPath != "")
          audio->Play(next.audioPath, false, 2.0f);
        lastAudio = next.audioPath;
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
          case Character::EmptyBod:
            ds.ShowMessage(next.text, {Anubis, EmptyBook});
            break;
          case Character::FullBod:
            ds.ShowMessage(next.text, {Anubis, FullBook});
            break;
          case Character::Title:
            showingTitle = true;
            ds.ShowMessage("");
            break;
          case Character::Credit:
            showingEnd = true;
            ds.ShowMessage("");
          case Character::None:
            ds.ShowMessage(next.text);
            break;
        }

      }
    }
    else if(lastAudio != "")
    {
      if(!audio->Playing(lastAudio))
      {
        ds.skipMessage();
      }
    }
    if(showingTitle || showingEnd)
    {
      if(showingEnd)
        Background.setColour(glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
      Background.setRect(glmhelper::correctRectWithCamera(glm::vec4(0, 0, settings::TARGET_WIDTH, settings::TARGET_HEIGHT), camRect, camScale));
      Background.Update(camRect);
      EmptyBookTitle.setRect(
        glmhelper::correctRectWithCamera(
          glm::vec4(300.0f, 200.0f, EmptyBook.getTextureDim().x/7, EmptyBook.getTextureDim().y/7),
          camRect,
          camScale
        )
      );
      EmptyBookTitle.Update(camRect);
    }
    ds.Update(timer, controls, camRect, camScale);
  }

  void CutsceneManager::Draw(Render *render)
  {
    if(showingTitle)
    {
      Background.Draw(render);
      EmptyBookTitle.Draw(render);
    }
    else if(showingEnd)
    {
      Background.Draw(render);
      render->DrawString(font, "Thanks For Playing!",
          glm::vec2((100*ds.lastScale()) + ds.lastCamRect().x,
                    (100)*ds.lastScale() + ds.lastCamRect().y),
          90.0f*ds.lastScale(), 4.0f, glm::vec4(1.0f)
      );
      render->DrawString(font, "Music - Mick Cooke (MakeFire Music)",
          glm::vec2((100*ds.lastScale()) + ds.lastCamRect().x,
                    (300)*ds.lastScale() + ds.lastCamRect().y),
          80.0f*ds.lastScale(), 4.0f, glm::vec4(1.0f)
      );
      render->DrawString(font, "Art - Thanos Gramosis",
          glm::vec2((100*ds.lastScale()) + ds.lastCamRect().x,
                    (400)*ds.lastScale() + ds.lastCamRect().y),
          80.0f*ds.lastScale(), 4.0f, glm::vec4(1.0f)
      );
      render->DrawString(font, "Sound fx - Paul James (Wafer Audio)",
          glm::vec2((100*ds.lastScale()) + ds.lastCamRect().x,
                    (500)*ds.lastScale() + ds.lastCamRect().y),
          80.0f*ds.lastScale(), 4.0f, glm::vec4(1.0f)
      );
      render->DrawString(font, "Voice - Paulina Ramirez (Lady Yami #3939)",
          glm::vec2((100*ds.lastScale()) + ds.lastCamRect().x,
                    (600)*ds.lastScale() + ds.lastCamRect().y),
          80.0f*ds.lastScale(), 4.0f, glm::vec4(1.0f)
      );
      render->DrawString(font, "Code - Noam Zeise",
          glm::vec2((100*ds.lastScale()) + ds.lastCamRect().x,
                    (700)*ds.lastScale() + ds.lastCamRect().y),
          80.0f*ds.lastScale(), 4.0f, glm::vec4(1.0f)
      );
      render->DrawString(font, "Click to exit",
          glm::vec2((1400*ds.lastScale()) + ds.lastCamRect().x,
                    (1040)*ds.lastScale() + ds.lastCamRect().y),
          50.0f*ds.lastScale(), 4.0f, glm::vec4(1.0f)
      );
    }
    else
    {
      ds.Draw(render);
    }
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
          case 'E':
            nextDialogue.character = Character::EmptyBod;
            break;
          case 'F':
            nextDialogue.character = Character::FullBod;
            break;
          case 'T':
            nextDialogue.character = Character::Title;
            break;
          case 'C':
            nextDialogue.character = Character::Credit;
            break;
          default:
            nextDialogue.character = Character::None;
            break;
        }
        nextDialogue.audioPath = "";
        if(line.size() > 5)
        {
          if(nextDialogue.character == Character::Title)
            nextDialogue.audioPath = "audio/music/" + line.substr(2, line.size() - 2);
          else
            nextDialogue.audioPath = "audio/vo/" + line.substr(2, line.size() - 2);
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
