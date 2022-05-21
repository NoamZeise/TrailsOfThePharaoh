#version 430 core
layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTexCoord;

layout (location = 0) out vec2 outTexCoord;
layout (location = 1) out vec3 outFragPos;
layout (location = 2) out vec3 outNormal;

const int MAX_3D_INSTANCE = 100;

layout (std430, binding = 2) buffer perInstanceModelData
{
  mat4 model[MAX_3D_INSTANCE];
};

layout (std430, binding = 3) buffer perInstanceNormalData
{
  mat4 normalMat[MAX_3D_INSTANCE];
};



uniform mat4 projection;
uniform mat4 view;

void main()
{
  outTexCoord = inTexCoord;

  vec4 fragPos = view * model[gl_InstanceID] * vec4(inPos, 1.0);
  outNormal = mat3(normalMat[gl_InstanceID]) * inNormal;

  gl_Position = projection * fragPos;
  outFragPos = vec3(fragPos) / fragPos.w;
}
