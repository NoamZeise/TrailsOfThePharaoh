#version 430

in vec2 TexCoords;
flat in int instanceID;

out vec4 colour;

const int MAX_2D_INSTANCE = 1000;

layout (std430, binding = 5) buffer perInstanceTexOffsetData
{
  vec4 texOffset[MAX_2D_INSTANCE];
};

const int MAX_2D_RAYS = 100;

layout (std430, binding = 6) buffer pirayp1
{
  vec2 rayp1[MAX_2D_RAYS];
};

layout (std430, binding = 7) buffer pirayp2
{
  vec2 rayp2[MAX_2D_RAYS];
};

layout (std430, binding = 8) buffer piraydist
{
  float raydist[MAX_2D_RAYS];
};

uniform sampler2D image;
uniform vec4 spriteColour;
uniform bool enableTex;
uniform float time;

void main()
{
  vec2 coord = TexCoords;
  coord.x *= texOffset[instanceID].z;
  coord.y *= texOffset[instanceID].w;
  coord.x += texOffset[instanceID].x;
  coord.y += texOffset[instanceID].y;
  if(enableTex)
      colour = texture(image, coord) * spriteColour;
  else
      colour = spriteColour;

  float attenuation = 0.0f;
  for(int i = 0; i < MAX_2D_RAYS; i++)
  {

    if(raydist[i] == 0)
      break;

    float dist = 0.0f;

    vec2 lineVec = (rayp2[i] - rayp1[i]);
    float l2 = (lineVec.x* lineVec.x) + (lineVec.y*lineVec.y);

    if(l2 == 0.0f)
      dist = distance(rayp1[i], gl_FragCoord.xy);
    else
    {
      float t = max(0, min(1, dot(gl_FragCoord.xy - rayp1[i], rayp2[i] - rayp1[i]) / l2));
      vec2 projection = rayp1[i] + (rayp2[i] - rayp1[i])*t;
      float fromStart = distance(rayp1[i], gl_FragCoord.xy);
      float correction = abs(sin(fromStart * 0.01f - time))*0.5f + 0.3f;

      dist = distance(gl_FragCoord.xy, projection) / correction;
    }

    attenuation += 1.0f / (1.0f + 0.5f * dist + 0.1f * dist * dist);
  }

  colour += vec4(1.0f, 1.0f, 0.0f, 0.0f) * attenuation;


  if(colour.w == 0)
    discard;
}
