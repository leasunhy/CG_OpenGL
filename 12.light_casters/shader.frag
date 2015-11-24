#version 330 core

struct Material {
  sampler2D diffuse;
  sampler2D specular;
  float shininess;
};

struct Light {
  vec3 position;
  vec3 direction;
  float cutoff;
  float outerCutoff;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

out vec4 color;

uniform Light light;
uniform Material material;
uniform vec3 ViewPos;

void main()
{
  vec3 lightDir = normalize(light.position - FragPos);
  float theta = dot(lightDir, normalize(-light.direction));
  float epsilon = light.cutoff - light.outerCutoff;
  float intensity = clamp((theta - light.outerCutoff) / epsilon, 0.0, 1.0);
  // ambient light
  float ambinetStrength = 0.1f;
  vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));

  // diffuse light
  vec3 norm = normalize(Normal);
  //vec3 lightDir = normalize(-light.direction);
  float diff = max(dot(lightDir, norm), 0.0);
  vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));

  // specular light
  float specularStrength = 0.5f;
  vec3 viewDir = normalize(ViewPos - FragPos);
  vec3 reflectDir = reflect(-lightDir, norm);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
  vec3 specular = light.specular * (spec * vec3(texture(material.specular, TexCoords)));

  vec3 result = ambient + (diffuse + specular) * intensity;
  color = vec4(result, 1.0f);
}
