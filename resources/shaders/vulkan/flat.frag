#version 450

layout(push_constant) uniform fragconstants
{
    layout(offset = 128) vec4 colour;
    vec4 texOffset;
    uint texID;
} pc;


layout(set = 2, binding = 0) uniform sampler texSamp;
layout(set = 2, binding = 1) uniform texture2D textures[200];

layout(set = 3, binding = 0) readonly buffer PerInstanceBuffer {
    vec4 colour;
    vec4 texOffset;
    uint texID;
} pib[2000];

const int RAY_COUNT = 100;
layout(set = 4, binding = 0) readonly buffer PerFrameLightPoints{
  vec2 p1;
  vec2 p2;
  float distance;
} rays[RAY_COUNT];

layout(location = 0) in vec3 inTexCoord;
layout(location = 1) in vec3 inVertPos;
layout(location = 2) in float time;

layout(location = 0) out vec4 outColour;


vec4 calcColour(vec4 texOffset, vec4 colour, uint texID)
{
    vec2 coord = inTexCoord.xy;
    coord.x *= texOffset.z;
    coord.y *= texOffset.w;
    coord.x += texOffset.x;
    coord.y += texOffset.y;

    vec4 col = texture(sampler2D(textures[texID], texSamp), coord) * colour;

    float attenuation = 0.0f;
    for(int i = 0; i < RAY_COUNT; i++)
    {

      if(rays[i].distance == 0)
        break;

      float dist = 0.0f;

      vec2 lineVec = (rays[i].p2 - rays[i].p1);
      float l2 = (lineVec.x* lineVec.x) + (lineVec.y*lineVec.y);

      if(l2 == 0.0f)
        dist = distance(rays[i].p1, gl_FragCoord.xy);
      else
      {
        float t = max(0, min(1, dot(gl_FragCoord.xy - rays[i].p1, rays[i].p2 - rays[i].p1) / l2));
        vec2 projection = rays[i].p1 + (rays[i].p2 - rays[i].p1)*t;
        float correction = 1.0f;
      //  if(projection != rays[i].p2)
      //  {
          float fromStart = distance(rays[i].p1, gl_FragCoord.xy);
          correction = abs(sin(fromStart * 0.01f - time))*0.5f + 0.3f;
        //}
        dist = distance(gl_FragCoord.xy, projection) / correction;
      }

      attenuation += 1.0f / (1.0f + 0.3f * dist + 0.05f * dist * dist);
    }

    col += vec4(1.0f, 1.0f, 0.7882352941f, 0.0f) * attenuation;

    if(col.w == 0)
        discard;
    return col;
}

void main()
{
    if(pc.texID == 0)
    {
        uint index = uint(inTexCoord.z);
        outColour = calcColour(pib[index].texOffset, pib[index].colour, pib[index].texID);
    }
    else
    {
      outColour = calcColour(pc.texOffset, pc.colour, pc.texID);
    }
}
