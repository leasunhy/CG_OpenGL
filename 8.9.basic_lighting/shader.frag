#version 330 core

in vec3 Normal;
in vec3 FragWorldPos;

out vec4 color;

uniform vec3 viewPos;
uniform vec3 lightPos;
uniform vec3 objectColor;
uniform vec3 lightColor;

void main()
{
  // ambient light
  float ambinetStrength = 0.1f;
  vec3 ambient = lightColor * ambinetStrength;

  // diffuse light
  vec3 norm = normalize(Normal);
  vec3 lightDir = normalize(lightPos - FragWorldPos);
  vec3 diffuse = max(dot(lightDir, norm), 0.0) * lightColor;

  // specular light
  float specularStrength = 0.5f;
  vec3 viewDir = normalize(viewPos - FragWorldPos);
  vec3 reflectDir = reflect(-lightDir, norm);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
  vec3 specular = specularStrength * spec * lightColor;

  vec3 result = (ambient + diffuse + specular) * objectColor;
  color = vec4(result, 1.0f);
}