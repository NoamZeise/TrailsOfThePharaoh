#ifndef APP_H
#define APP_H

#include <atomic>
#include <iostream>
#include <thread>

#ifdef GFX_ENV_VULKAN
  #ifndef GLFW_INCLUDE_VULKAN
  #define GLFW_INCLUDE_VULKAN
  #endif
  #ifndef GLM_FORCE_DEPTH_ZERO_TO_ONE
  #define GLM_FORCE_DEPTH_ZERO_TO_ONE
  #endif
#endif

#ifdef GFX_ENV_OPENGL
  #include <glad/glad.h>
#endif

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <audio.h>
#include <input.h>
#include <timer.h>
#include <glmhelper.h>

#include "renderer-include.h"

#include <config.h>

#include "camera.h"

#include "game/map/level.h"
#include "game/sprites/sprite.h"
#include "game/sprites/UI/button.h"
#include "game/sprites/UI/text_button.h"
#include "game/sprites/UI/cutscene_manager.h"

//#define TIME_APP_DRAW_UPDATE
//#define MULTI_UPDATE_ON_SLOW_DRAW

class App {
public:
  App();
  ~App();
  void run();
  void resize(int windowWidth, int windowHeight);

  static void framebuffer_size_callback(GLFWwindow *window, int width, int height);
  static void mouse_callback(GLFWwindow *window, double xpos, double ypos);
  static void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
  static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
  static void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);
  static void error_callback(int error, const char *description);

  Input::Raw input;

private:
  void loadAssets();
  void loadMaps();

  void preUpdate();
  void update();
  void postUpdate();
  void draw();
  void nextMap();

  glm::vec2 correctedPos(glm::vec2 pos);
  glm::vec2 appToScreen(glm::vec2 pos);
  glm::vec2 correctedMouse();

  GLFWwindow *mWindow;
  Render *mRender;
  int mWindowWidth, mWindowHeight;
  Input::Raw previousInput;
  Input::Controls controls;
  Timer timer;
  float timeSinceStart = 0.0f;

  Audio::Manager audioManager;

  std::thread submitDraw;
  std::atomic<bool> finishedDrawSubmit;

  Camera::RoomFollow2D camera;
  glm::vec2 target = glm::vec2(0, 0);
  float scale  = 2.0f;

  bool inGame = false;

  Resource::Texture pixelTex;
  Resource::Font gameFont;
  Sprite currentCursor;
  Sprite cursor;
  Sprite rotateCursor;
  Sprite rotateCursorOn;
  Sprite moveCursor;
  Sprite moveCursorOn;
  std::vector<Level> levels;
  int currentLevelIndex;
  Level currentLevel;
  TextButton retryButton;
  bool lvlShowContinue = false;
  TextButton continueButton;

  bool inLevelTransition = false;
  bool inLevelDialogueTransition = false;
  float LevelTransitionDelay = 1000.0f;
  float LevelTransitionTimer = 0.0f;
  Level nextLevel;
  glm::vec2 startTransitionCamOff;

  bool inDialogue = true;

  bool lastCutsceneWatched = false;

  bool allLevelsComplete = false;

  CutsceneManager csManager;

  gh::Random rand;

  int lastAmbientIndex = -1;
  std::vector<std::string> ambientMusicTracks = {
    "audio/music/Trials of The Pharaoh Gameplay1.ogg",
    "audio/music/Trials of The Pharaoh Gameplay2.ogg",
    //"audio/music/Trials of The Pharaoh Gameplay3.ogg",
    "audio/music/Trials of The Pharaoh Gameplay4.ogg"
  };
};

#endif
