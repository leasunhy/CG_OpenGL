#version 330 core

struct Material {
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  float shininess;
};

struct Light {
  vec3 position;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

in vec3 Normal;
in vec3 FragPos;

out vec4 color;

uniform Light light;
uniform Material material;

void main()
{
  // ambient light
  float ambinetStrength = 0.1f;
  vec3 ambient = light.ambient * material.ambient;

  // diffuse light
  vec3 norm = normalize(Normal);
  vec3 lightDir = normalize(light.position - FragPos);
  vec3 diffuse = light.diffuse * max(dot(lightDir, norm), 0.0) * material.diffuse;

  // specular light
  float specularStrength = 0.5f;
  vec3 viewDir = normalize(-FragPos);
  vec3 reflectDir = reflect(-lightDir, norm);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
  vec3 specular = light.specular * (spec * material.specular);

  vec3 result = ambient + diffuse + specular;
  color = vec4(result, 1.0f);
}
