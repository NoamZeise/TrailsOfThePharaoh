#include "app.h"

App::App()
{
  glfwSetErrorCallback(error_callback);
  if (!glfwInit())
    throw std::runtime_error("failed to initialise glfw!");

  const GLFWvidmode* videoMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
  mWindowWidth = videoMode->width*0.95;
  mWindowHeight = videoMode->height*0.95;

  Render::SetGLFWWindowHints();

  #ifdef GFX_ENV_VULKAN
  mWindow = glfwCreateWindow(mWindowWidth, mWindowHeight, "Trials of The Pharaoh", nullptr, nullptr);
  #endif
  #ifdef GFX_ENV_OPENGL
  mWindow = glfwCreateWindow(mWindowWidth, mWindowHeight, "Trials of The Pharaoh", nullptr, nullptr);
  #endif
  if (!mWindow)
  {
    glfwTerminate();
    throw std::runtime_error("failed to create glfw window!");
  }
  glfwSetWindowPos(mWindow, mWindowHeight*0.025f, mWindowHeight*0.025f);
  GLFWimage winIcon[1];
	winIcon[0].pixels = stbi_load("textures/icon.png", &winIcon[0].width, &winIcon[0].height, 0, 4); //rgba channels
	glfwSetWindowIcon(mWindow, 1, winIcon);
  glfwSetWindowUserPointer(mWindow, this);
  glfwSetFramebufferSizeCallback(mWindow, framebuffer_size_callback);
  glfwSetCursorPosCallback(mWindow, mouse_callback);
  glfwSetScrollCallback(mWindow, scroll_callback);
  glfwSetKeyCallback(mWindow, key_callback);
  glfwSetMouseButtonCallback(mWindow, mouse_button_callback);
  glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
  glfwSetInputMode(mWindow, GLFW_RAW_MOUSE_MOTION, glfwRawMouseMotionSupported());

  int width = mWindowWidth;
  int height = mWindowHeight;
  if (settings::USE_TARGET_RESOLUTION)
  {
    width = settings::TARGET_WIDTH;
    height = settings::TARGET_HEIGHT;
  }

  mRender = new Render(mWindow, glm::vec2(width, height));

  if (settings::FIXED_RATIO)
    glfwSetWindowAspectRatio(mWindow, width, height);

  loadAssets();

  finishedDrawSubmit = true;
}

App::~App()
{
  if (submitDraw.joinable())
    submitDraw.join();
  delete mRender;
  glfwTerminate();
}

void App::loadAssets()
{
  gameFont = mRender->LoadFont("textures/SF Eccentric Opus.ttf");
  cursor = Sprite(mRender->LoadTexture("textures/ui/cursor.png"), 4.1f);
  moveCursor = Sprite(mRender->LoadTexture("textures/ui/moveCursor.png"), 4.1f);
  rotateCursor = Sprite(mRender->LoadTexture("textures/ui/rotateCursor.png"), 4.1f);
  moveCursorOn = Sprite(mRender->LoadTexture("textures/ui/moveCursorOn.png"), 4.1f);
  rotateCursorOn = Sprite(mRender->LoadTexture("textures/ui/rotateCursorOn.png"), 4.1f);
  pixelTex = mRender->LoadTexture("textures/pixel.png");

  auto btnSprite = Sprite(mRender->LoadTexture("textures/ui/button.png"),glm::vec4(0),1.0f);

  btnSprite.setDepth(2.0f);
  btnSprite.setRect(
    glm::vec4(
      settings::TARGET_WIDTH - 215,
      settings::TARGET_HEIGHT - 105,
      205, 100)
  );
  retryButton = TextButton(
    btnSprite,
    true,
    "Reset",
    gameFont
  );

  btnSprite.setDepth(3.0f);
  btnSprite.setRect(
    glm::vec4(
      settings::TARGET_WIDTH/2 - 320/2,
      settings::TARGET_HEIGHT/2 - 100/2,
      320, 100)
  );
  continueButton = TextButton(
    btnSprite,
    true,
    "Continue",
    gameFont
  );


  loadMaps();

  for(const auto &f: ambientMusicTracks)
    audioManager.LoadAudioFile(f);

  csManager = CutsceneManager(mRender, gameFont, &audioManager);
  audioManager.Play("audio/music/Trials of The Pharaoh Intro.ogg", false, 1.2f);
  csManager.PlayCutscene("dialogue/0.txt");

  inGame = false;
  inDialogue = true;



  mRender->EndResourceLoad();
}

void App::loadMaps()
{
  currentLevelIndex = -1;  //1 less than desired index
  levels.push_back(Level("maps/1-two-movers.tmx", mRender, gameFont, &audioManager));
  levels.push_back(Level("maps/2-simple-movers-puzzle.tmx", mRender, gameFont, &audioManager));
  levels.push_back(Level("maps/3-split-intro.tmx", mRender, gameFont, &audioManager));
  levels.push_back(Level("maps/4-split-two.tmx", mRender, gameFont, &audioManager));
  levels.push_back(Level("maps/5-5-tilter-intermission.tmx", mRender, gameFont, &audioManager));
  levels.push_back(Level("maps/5-tilters.tmx", mRender, gameFont, &audioManager));
  levels.push_back(Level("maps/9-25-split-on-off-intro.tmx", mRender, gameFont, &audioManager));
  levels.push_back(Level("maps/6-more-tilters.tmx", mRender, gameFont, &audioManager));
  levels.push_back(Level("maps/7-light-hold-intro.tmx", mRender, gameFont, &audioManager));
  levels.push_back(Level("maps/8-light-hold-quick.tmx", mRender, gameFont, &audioManager));
  levels.push_back(Level("maps/9-three-hold.tmx", mRender, gameFont, &audioManager));
  levels.push_back(Level("maps/9-5-mirror-hold-intro.tmx", mRender, gameFont, &audioManager));
  levels.push_back(Level("maps/10-mirror-hold-intro.tmx", mRender, gameFont, &audioManager));
  levels.push_back(Level("maps/10-5-interim.tmx", mRender, gameFont, &audioManager));
  levels.push_back(Level("maps/11-mirror-hold-ext.tmx", mRender, gameFont, &audioManager));
}

void App::run()
{
  while (!glfwWindowShouldClose(mWindow))
  {
    update();
    if (mWindowWidth != 0 && mWindowHeight != 0)
      draw();
  }
}

void App::resize(int windowWidth, int windowHeight)
{
  if (submitDraw.joinable())
    submitDraw.join();
  this->mWindowWidth = windowWidth;
  this->mWindowHeight = windowHeight;
  if (mRender != nullptr && mWindowWidth != 0 && mWindowHeight != 0)
    mRender->FramebufferResize();
}

void App::preUpdate()
{
  glfwPollEvents();
  controls.Update(input, correctedMouse(), camera.getCameraOffset());
}

void App::update()
{
#ifdef TIME_APP_DRAW_UPDATE
  auto start = std::chrono::high_resolution_clock::now();
#endif

  preUpdate();

  float camspeed = 1.0f * scale;
  if(controls.Up())
    target.y -= camspeed * timer.FrameElapsed();
  if(controls.Down())
    target.y += camspeed * timer.FrameElapsed();
  if(controls.Left())
    target.x -= camspeed * timer.FrameElapsed();
  if(controls.Right())
    target.x += camspeed * timer.FrameElapsed();
  if(controls.Plus())
    scale -=  0.001f * timer.FrameElapsed();
  if(controls.Minus())
    scale +=  0.001f * timer.FrameElapsed();

  camera.setScale(scale);
  camera.Target(target, timer);

  std::vector<DS::ShaderStructs::ray2D> rays;
  currentCursor = cursor;

  if(csManager.isFinished() && !audioManager.Playing(ambientMusicTracks))
  {
    int index = (int)(rand.PositiveReal() * ambientMusicTracks.size());
    if(index == lastAmbientIndex)
      index = (int)(rand.PositiveReal() * ambientMusicTracks.size());
    audioManager.Play(ambientMusicTracks[index], false, 0.5f);
    lastAmbientIndex = index;
  }

  if(inLevelTransition && currentLevelIndex < levels.size())
  {
    if(LevelTransitionTimer == 0.0f)
    {
      startTransitionCamOff = camera.getCameraOffset();
    }
    LevelTransitionTimer += timer.FrameElapsed();
    if(LevelTransitionTimer > LevelTransitionDelay)
    {
      LevelTransitionTimer = 0.0f;
      inLevelTransition = false;
    }
    else if(LevelTransitionTimer > LevelTransitionDelay/2 && !inGame)
    {
      inGame = true;
      inLevelDialogueTransition = false;
      audioManager.Stop("audio/music/Trials of The Pharaoh Intro.ogg");
      if(currentLevelIndex == levels.size() - 1)
      {

        if(!lastCutsceneWatched)
        {
          audioManager.Stop(ambientMusicTracks[lastAmbientIndex]);
          audioManager.Play("audio/music/Trials of The Pharaoh End.ogg", false, 1.3f);
          csManager.PlayCutscene("dialogue/end.txt");
        }
        lastCutsceneWatched = true;
        inDialogue = true;
        inGame = false;

      }
      else
        nextMap();
    }
  }
  else if(inLevelTransition)
  {
    inLevelTransition = false;
    inLevelDialogueTransition = false;
    inGame = true;
    nextMap();
  }

  if(inGame)
  {
    currentLevel.Update(camera.getCameraArea(), scale, timer, controls);

    if(!inLevelTransition)
    {
    for(auto &mapRay : currentLevel.shaderRays)
    {
      DS::ShaderStructs::ray2D ray = mapRay;
      ray.p1.x = ray.p1.x - camera.getCameraArea().x;
      ray.p1.y = ray.p1.y - camera.getCameraArea().y;
      ray.p1 = appToScreen(ray.p1);
      ray.p2.x = ray.p2.x - camera.getCameraArea().x;
      ray.p2.y = ray.p2.y - camera.getCameraArea().y;
      ray.p2 = appToScreen(ray.p2);
      rays.push_back(ray);
    }
    }

    if(currentLevel.complete())
    {
      inLevelTransition = true;
      if(currentLevelIndex % 2 == 0)
        audioManager.Play("audio/music/Level Complete.ogg", false, 0.35f);
      else
        audioManager.Play("audio/music/Level Complete2.ogg", false, 0.35f);
      inGame = false;
    }

    retryButton.Update(camera.getCameraArea(), controls, scale);

    if(retryButton.Clicked())
    {
      inLevelTransition = true;
      inGame = false;
      currentLevelIndex--;
      //nextMap();
    }

    if(currentLevel.movedSel())
      currentCursor = moveCursor;
    if(currentLevel.rotatedSel())
      currentCursor = rotateCursor;

    if(currentLevel.moved())
    {
      currentCursor =  moveCursorOn;
    }
    if(currentLevel.rotated())
    {
      currentCursor = rotateCursorOn;
    }
  }
  else
  {
    if(lvlShowContinue)
    {
      continueButton.Update(camera.getCameraArea(), controls, scale);

      if(continueButton.Clicked())
      {
        lvlShowContinue = false;
        inLevelTransition = true;
      }
    }
    if(inDialogue)
    {
      csManager.Update(timer, controls, camera.getCameraArea(), camera.getScale());
      if(csManager.isFinished())
      {
        if(lastCutsceneWatched)
          glfwSetWindowShouldClose(mWindow, GLFW_TRUE);
        inDialogue = false;
        inLevelTransition = true;
        inLevelDialogueTransition = true;
        inGame = false;
      }
    }
  }

  mRender->set2DRayData(rays);

  auto cursorRect = cursor.getDrawRect();
  currentCursor.setRect(glm::vec4(controls.MousePos().x - cursorRect.z/2, controls.MousePos().y - cursorRect.w/2, cursorRect.z, cursorRect.w));
  currentCursor.Update(camera.getCameraArea());

  postUpdate();

#ifdef TIME_APP_DRAW_UPDATE
  auto stop = std::chrono::high_resolution_clock::now();
  std::cout << "update: "
            << std::chrono::duration_cast<std::chrono::microseconds>(stop -
                                                                     start)
                   .count()
            << " microseconds" << std::endl;
#endif
}

void App::postUpdate()
{
  timeSinceStart += timer.FrameElapsed();
  mRender->set2DViewMatrixAndScaleAndTime(camera.getViewMat(), camera.getScale(), timeSinceStart / 500);
  previousInput = input;
  input.offset = 0;
  timer.Update();
}

void App::nextMap()
{
  currentLevelIndex++;
  if(currentLevelIndex >= levels.size())
  {
    glfwSetWindowShouldClose(mWindow, GLFW_TRUE);
    currentLevelIndex--;
    allLevelsComplete = true;
  }
  else
  {
    currentLevel = levels[currentLevelIndex];
  }
  auto rect = currentLevel.getMapRect();
  target = glm::vec2(rect.x + rect.z/2, rect.y +  rect.w/2);
  camera.SetCameraOffset(target);
  camera.setCameraMapRect(rect);
  scale = rect.z / settings::TARGET_WIDTH;
  if(rect.w / settings::TARGET_HEIGHT > scale)
    scale = rect.w / settings::TARGET_HEIGHT;
}

void App::draw()
{
#ifdef TIME_APP_DRAW_UPDATE
  auto start = std::chrono::high_resolution_clock::now();
#endif

#ifdef MULTI_UPDATE_ON_SLOW_DRAW
  if (!finishedDrawSubmit)
    return;
  finishedDrawSubmit = false;
#endif

  if(submitDraw.joinable())
    submitDraw.join();

  mRender->Begin2DDraw();

  mRender->DrawQuad(pixelTex,
    glmhelper::calcMatFromRect(glm::vec4(0, 0, currentLevel.getMapRect().z, currentLevel.getMapRect().w), 0.0f, -1.0f),
    glm::vec4(0.6235, 0.5294f, 0.4196f, 1.0f)
  );

  if(inLevelTransition)
  {
    mRender->DrawQuad(
      pixelTex,
      glmhelper::calcMatFromRect(glm::vec4(
        camera.getCameraOffset().x +   settings::TARGET_WIDTH * camera.getScale() * (1 - LevelTransitionTimer*2.0f/LevelTransitionDelay),
        camera.getCameraOffset().y,
        settings::TARGET_WIDTH * camera.getScale(), settings::TARGET_HEIGHT * camera.getScale()),
        0.0f, 7.0f),
      glm::vec4(settings::BG_R, settings::BG_G, settings::BG_B, 1.0f)
    );
  }

  currentLevel.Draw(mRender);

  currentCursor.Draw(mRender);

  if(inDialogue || inLevelDialogueTransition)
    csManager.Draw(mRender);

  retryButton.Draw(mRender);

  if(lvlShowContinue)
  {
    mRender->DrawQuad(pixelTex,
      glmhelper::calcMatFromRect(glm::vec4(camera.getCameraOffset().x, camera.getCameraOffset().y, settings::TARGET_WIDTH*scale, settings::TARGET_HEIGHT*scale), 0.0f, 2.5f),
      glm::vec4(0.0f, 0.0f, 0.0f, 0.5f));
    continueButton.Draw(mRender);
    mRender->DrawString(gameFont, "Level Complete!",
      glm::vec2(
        (settings::TARGET_WIDTH/2.0f - 180.0f)*scale + camera.getCameraArea().x,
        (settings::TARGET_HEIGHT/2.0f - 300.0f)*scale + camera.getCameraArea().y),
      50.0f*scale, 3.0f, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
  }

  if(allLevelsComplete)
    mRender->DrawString(gameFont, "GAME COMPLETE", glm::vec2(500, 500), 200.0f, 3.0f, glm::vec4(1.0f));

#ifdef GFX_ENV_VULKAN
  submitDraw =
      std::thread(&Render::EndDraw, mRender, std::ref(finishedDrawSubmit));
#endif

#ifdef GFX_ENV_OPENGL
 mRender->EndDraw(finishedDrawSubmit);
#endif

#ifdef TIME_APP_DRAW_UPDATE
  auto stop = std::chrono::high_resolution_clock::now();
  std::cout << "draw: "
            << std::chrono::duration_cast<std::chrono::microseconds>(stop -
                                                                     start)
                   .count()
            << " microseconds" << std::endl;
#endif
}

glm::vec2 App::correctedPos(glm::vec2 pos)
{
  if (settings::USE_TARGET_RESOLUTION)
    return glm::vec2(
        pos.x * ((float)settings::TARGET_WIDTH*scale / (float)mWindowWidth),
        pos.y * ((float)settings::TARGET_HEIGHT*scale / (float)mWindowHeight));

  return glm::vec2(pos.x, pos.y);
}

glm::vec2 App::appToScreen(glm::vec2 pos)
{
  #ifdef OGL_RENDER_H
  //std::cout << (float)mWindowWidth / (float)mWindowHeight << std::endl;
  //wRatio = wRatio > 1.0f ? 1.0f : wRatio;
  float ratio = (float)mWindowWidth  / (float)mWindowHeight;
  float proper = (float)settings::TARGET_WIDTH / (float)settings::TARGET_HEIGHT;
  float diff = ratio/proper;
	return glm::vec2((pos.x / scale) * ((float)mWindowWidth / (float)settings::TARGET_WIDTH) / diff,
    mWindowHeight  - ((pos.y / scale) * ((float)mWindowHeight / (float)settings::TARGET_HEIGHT)));
  #else
	return glm::vec2(pos.x / scale, pos.y / scale );
  #endif
}

glm::vec2 App::correctedMouse()
{
  return correctedPos(glm::vec2(input.X, input.Y));
}

/*
 *       GLFW CALLBACKS
 */

void App::framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
  auto app = reinterpret_cast<App *>(glfwGetWindowUserPointer(window));
  app->resize(width, height);
}

void App::mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
  App *app = reinterpret_cast<App *>(glfwGetWindowUserPointer(window));
  app->input.X = xpos;
  app->input.Y = ypos;
}
void App::scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
  App *app = reinterpret_cast<App *>(glfwGetWindowUserPointer(window));
  app->input.offset = yoffset;
}

void App::key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
  App *app = reinterpret_cast<App *>(glfwGetWindowUserPointer(window));
  if (key >= 0 && key < 1024) {
    if (action == GLFW_PRESS) {
      app->input.Keys[key] = true;
    } else if (action == GLFW_RELEASE) {
      app->input.Keys[key] = false;
    }
  }
}

void App::mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
  App *app = reinterpret_cast<App *>(glfwGetWindowUserPointer(window));

  if (button >= 0 && button < 8) {
    if (action == GLFW_PRESS) {
      app->input.Buttons[button] = true;
    } else if (action == GLFW_RELEASE) {
      app->input.Buttons[button] = false;
    }
  }
}

void App::error_callback(int error, const char *description)
{
  throw std::runtime_error(description);
}
