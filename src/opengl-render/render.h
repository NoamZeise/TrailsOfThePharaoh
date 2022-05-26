#ifndef OGL_RENDER_H
#define OGL_RENDER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include <stdexcept>
#include <iostream>
#include <string>
#include <atomic>
#include <vector>

#include "shader.h"
#include <config.h>
#include "resources/resources.h"
#include "resources/vertex_data.h"
#include "resources/texture_loader.h"
#include "resources/font_loader.h"
#include "resources/model_loader.h"

//match in shaders
const int MAX_2D_DRAWS = 2000;
const int MAX_3D_DRAWS = 100;

const int MAX_2D_BATCH = 1000;
const int MAX_3D_BATCH = 100;

const int MAX_2D_RAYS = 100;

namespace DS
{
	namespace ShaderStructs
	{
		struct ray2D
		{
			glm::vec2 p1;
			glm::vec2 p2;
			float magnitude;
		};
	}
}

class Render
{
public:
	Render(GLFWwindow* window, glm::vec2 target);
	~Render();
  static void SetGLFWWindowHints()
  {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  }
	void set3DViewMatrixAndFov(glm::mat4 view, float fov);
	void set2DViewMatrixAndScale(glm::mat4 view, float scale);

	Resource::Texture LoadTexture(std::string filepath);
  Resource::Model LoadModel(std::string filepath);
	Resource::Font LoadFont(std::string filepath);
	void EndResourceLoad() { }

  void Begin2DDraw();
  void Begin3DDraw();
	void DrawModel(Resource::Model model, glm::mat4 modelMatrix, glm::mat4 normalMat);
	void DrawQuad(Resource::Texture texture, glm::mat4 modelMatrix, glm::vec4 colour, glm::vec4 texOffset);
	void DrawQuad(Resource::Texture texture, glm::mat4 modelMatrix, glm::vec4 colour);
	void DrawQuad(Resource::Texture texture, glm::mat4 modelMatrix);
	void DrawString(Resource::Font font, std::string text, glm::vec2 position, float size, float depth, glm::vec4 colour, float rotate);
	void DrawString(Resource::Font font, std::string text, glm::vec2 position, float size, float depth, glm::vec4 colour);
  void EndDraw(std::atomic<bool>& submit);

	  void set2DRayData(std::vector<DS::ShaderStructs::ray2D> &setRays) {
	    int range = setRays.size();
	    if(setRays.size() > MAX_2D_RAYS)
	    {
	      range = MAX_2D_RAYS;
	      std::cerr << ("sent rays larger than capacity!") << std::endl;
	    }
	    for(int i = 0; i < range; i++)
			{
			  rayp1Data[i] = setRays[i].p1;
			  rayp2Data[i] = setRays[i].p2;
			  raydistData[i] = setRays[i].magnitude;
			}
	    if(range < MAX_2D_RAYS)
	    {
	      rayp1Data[range] = glm::vec2(-1);
	      raydistData[range] = 0.0f;
	    }
	  }

	void FramebufferResize();

private:

	void draw3DBatch(int drawCount, Resource::Model model);
	void draw2DBatch(int drawCount, Resource::Texture texture, glm::vec4 currentColour);

	struct LightingParameters
	{
		LightingParameters()
		{
			ambient = glm::vec4(1.0f, 1.0f, 1.0f, 0.6f);
    	diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 0.7f);
    	specular = glm::vec4(0.1f, 0.1f, 0.1f, 5.0f);
    	direction = glm::vec4(0.3f, -0.3f, -0.5f, 0.0f);
		}

		alignas(16) glm::vec4 ambient;
		alignas(16) glm::vec4 diffuse;
		alignas(16) glm::vec4 specular;
		alignas(16) glm::vec4 direction;
	};

	GLFWwindow* window;
	glm::vec2 targetResolution;

  Shader* blinnPhongShader;
	Shader* flatShader;

  int width;
  int height;

  glm::mat4 proj2D;
  glm::mat4 view2D;
	float scale = 1.0f;

  glm::mat4 proj3D;
  glm::mat4 view3D;
	float fov;

  VertexData* quad;

  Resource::TextureLoader* textureLoader;
	Resource::FontLoader* fontLoader;
  Resource::ModelLoader* modelLoader;

  struct Draw2D
  {
    Draw2D() {}
    Draw2D(Resource::Texture tex, glm::mat4 model, glm::vec4 colour, glm::vec4 texOffset);
    Resource::Texture tex;
    glm::mat4 model;
    glm::vec4 colour;
    glm::vec4 texOffset;
  };
  struct Draw3D
  {
    Draw3D() {}
    Draw3D(Resource::Model model, glm::mat4 modelMatrix, glm::mat4 normalMatrix);

    Resource::Model model;
    glm::mat4 modelMatrix;
    glm::mat4 normalMatrix;
  };

  unsigned int current2DDraw = 0;
  Draw2D draw2DCalls[MAX_2D_DRAWS];
  unsigned int current3DDraw = 0;
  Draw3D draw3DCalls[MAX_3D_DRAWS];

	glm::mat4 perInstance2DModel[MAX_2D_BATCH];
	glm::vec4 perInstance2DTexOffset[MAX_2D_BATCH];
	GLuint model2DSSBO;
	GLuint texOffset2DSSBO;

	glm::mat4 perInstance3DModel[MAX_3D_BATCH];
	glm::mat4 perInstance3DNormal[MAX_3D_BATCH];
	GLuint model3DSSBO;
	GLuint normal3DSSBO;

	glm::vec2 rayp1Data[MAX_2D_RAYS];
	glm::vec2 rayp2Data[MAX_2D_RAYS];
	float raydistData[MAX_2D_RAYS];
	GLuint rayp1SSBO;
	GLuint rayp2SSBO;
	GLuint raydistSSBO;
};





#endif
