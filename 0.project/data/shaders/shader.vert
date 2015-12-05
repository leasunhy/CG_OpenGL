#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;
out vec4 FragPosLightSpace;


uniform mat4 model;
uniform mat3 normalMatrix;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

void main()
{
  gl_Position = projection * view * model * vec4(position, 1.0f);

  Normal = transpose(inverse(mat3(model))) * normal;
  FragPos = vec3(model * vec4(position, 1.0f));
  TexCoords = vec2(texCoords.x, 1.0 - texCoords.y);

  FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);
}
