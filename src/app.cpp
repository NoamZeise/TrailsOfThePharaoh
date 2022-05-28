#include "app.h"

App::App()
{

  mWindowWidth = settings::INITIAL_WINDOW_WIDTH;
  mWindowHeight = settings::INITIAL_WINDOW_HEIGHT;

  glfwSetErrorCallback(error_callback);
  if (!glfwInit())
    throw std::runtime_error("failed to initialise glfw!");

  Render::SetGLFWWindowHints();

  #ifdef GFX_ENV_VULKAN
  mWindow = glfwCreateWindow(mWindowWidth, mWindowHeight, "Vulkan App", nullptr, nullptr);
  #endif
  #ifdef GFX_ENV_OPENGL
  mWindow = glfwCreateWindow(mWindowWidth, mWindowHeight, "OpenGL App", nullptr, nullptr);
  #endif
  if (!mWindow)
  {
    glfwTerminate();
    throw std::runtime_error("failed to create glfw window!");
  }

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
  nextMap();

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
  cursor = Sprite(mRender->LoadTexture("textures/ui/cursor.png"), 4.0f);
  moveCursor = Sprite(mRender->LoadTexture("textures/ui/moveCursor.png"), 4.0f);
  rotateCursor = Sprite(mRender->LoadTexture("textures/ui/rotateCursor.png"), 4.0f);
  moveCursorOn = Sprite(mRender->LoadTexture("textures/ui/moveCursorOn.png"), 4.0f);
  rotateCursorOn = Sprite(mRender->LoadTexture("textures/ui/rotateCursorOn.png"), 4.0f);
  pixelTex = mRender->LoadTexture("textures/pixel.png");

  testCharacter = Sprite(mRender->LoadTexture("textures/ui/character.png"), 3.2f);
  dialogue = DialogueSystem(
    Sprite(mRender->LoadTexture("textures/ui/dialogueBg.png"), glm::vec4(0, 0, 1600, 900), 3.5f),
    gameFont
  );

  dialogue.ShowMessage("Hello, I am looking for the way to the afterlife", &testCharacter);

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
  mRender->EndResourceLoad();
}

void App::loadMaps()
{
  currentLevelIndex = -1;  //1 less than desired index
  levels.push_back(Level("maps/1-two-movers.tmx", mRender, gameFont));
  levels.push_back(Level("maps/2-simple-movers-puzzle.tmx", mRender, gameFont));
  levels.push_back(Level("maps/3-split-intro.tmx", mRender, gameFont));
  levels.push_back(Level("maps/4-split-two.tmx", mRender, gameFont));
  levels.push_back(Level("maps/5-tilters.tmx", mRender, gameFont));
  levels.push_back(Level("maps/6-more-tilters.tmx", mRender, gameFont));
  levels.push_back(Level("maps/7-light-hold-intro.tmx", mRender, gameFont));
  levels.push_back(Level("maps/8-light-hold-quick.tmx", mRender, gameFont));
  levels.push_back(Level("maps/9-three-hold.tmx", mRender, gameFont));
  levels.push_back(Level("maps/10-mirror-hold-intro.tmx", mRender, gameFont));
  levels.push_back(Level("maps/11-mirror-hold-ext.tmx", mRender, gameFont));
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
  if (input.Keys[GLFW_KEY_ESCAPE] && !previousInput.Keys[GLFW_KEY_ESCAPE])
  {
    glfwSetWindowShouldClose(mWindow, GLFW_TRUE);
  }
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
      camera.SetCameraOffset(startTransitionCamOff);
      nextMap();
    }
    else if(LevelTransitionTimer < LevelTransitionDelay/2)
    {
      auto currentRect = currentLevel.getMapRect();
      float ratio = LevelTransitionTimer / LevelTransitionDelay;
      camera.SetCameraOffset(glm::vec2(startTransitionCamOff.x + settings::TARGET_WIDTH/2 + currentRect.z*ratio
        , startTransitionCamOff.y + settings::TARGET_HEIGHT/2));
    }
  }
  else if(inLevelTransition)
  {
    inLevelTransition = false;
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
      lvlShowContinue = true;
      inGame = false;
    }

    retryButton.Update(camera.getCameraArea(), controls, scale);

    if(retryButton.Clicked())
    {
      currentLevelIndex--;
      nextMap();
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

  }

      if(inDialogue)
      {
        dialogue.Update(timer, controls, camera.getCameraArea(), camera.getScale());
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
      glm::vec4(0.6235, 0.5294f, 0.4196f, 1.0f)
    );
  }

  currentLevel.Draw(mRender);
  if(inLevelTransition)
    nextLevel.Draw(mRender);

  if(inDialogue)
    dialogue.Draw(mRender);

  currentCursor.Draw(mRender);

  retryButton.Draw(mRender);

  if(lvlShowContinue)
  {
    mRender->DrawQuad(pixelTex, glmhelper::calcMatFromRect(glm::vec4(0, 0, settings::TARGET_WIDTH*scale, settings::TARGET_HEIGHT*scale), 0.0f, 2.5f), glm::vec4(0.0f, 0.0f, 0.0f, 0.5f));
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
