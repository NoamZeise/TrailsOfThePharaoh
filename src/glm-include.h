#ifndef GLM_INCLUDE_H
#define GLM_INCLUDE_H

#ifdef GFX_ENV_VULKAN
  #ifndef GLM_FORCE_DEPTH_ZERO_TO_ONE
  #define GLM_FORCE_DEPTH_ZERO_TO_ONE
  #endif
#endif
#include <glm/glm.hpp>

#endif
