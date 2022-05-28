#ifndef RENDERER_CONFIG_H
#define RENDERER_CONFIG_H


//#define NDEBUG
#define NO_ASSIMP

namespace settings
{
const bool SRGB = false;
const bool MIP_MAPPING = true;
const bool PIXELATED = false;
const bool VSYNC = true;
const bool MULTISAMPLING = true;
const bool SAMPLE_SHADING = true;

const bool FIXED_RATIO = true;
const bool USE_TARGET_RESOLUTION = true;
const int TARGET_WIDTH = 1920;
const int TARGET_HEIGHT = 1080;

const int INITIAL_WINDOW_WIDTH = 1600;
const int INITIAL_WINDOW_HEIGHT = 900;

const float BG_R = 117.0f/255.0f;
const float BG_G = 94.0f/255.0f;
const float BG_B = 82.0f/255.0f;


#ifndef NDEBUG
const bool ERROR_ONLY = true;
#endif
}


#endif
