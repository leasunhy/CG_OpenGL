#version 330 core

#define N_POINT_LIGHT 4

struct Material {
  float shininess;
};

struct DirLight {
  vec3 direction;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

struct PointLight {
  vec3 position;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;

  float constant;
  float linear;
  float quadratic;
};

struct SpotLight {
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
in vec4 Color;

out vec4 color;

uniform DirLight dirLight;
uniform SpotLight spotLight;
uniform PointLight pointLights[N_POINT_LIGHT];
uniform int pointLightCount;

uniform Material material;
uniform vec3 ViewPos;

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
  vec3 normal = normalize(Normal);
  vec3 viewDir = normalize(ViewPos - FragPos);

  vec3 result = vec3(0.0);
  result += calcDirLight(dirLight, normal, viewDir);
  for (int i = 0; i < pointLightCount; ++i)
    result += calcPointLight(pointLights[i], normal, FragPos, viewDir);
  result += calcSpotLight(spotLight, normal, FragPos, viewDir);

  color = vec4(result, 1.0);
}

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir) {
  vec3 lightDir = normalize(-light.direction);

  // ambient
  vec3 ambient = light.ambient * Color.rgb;

  // diffuse
  float diff = max(dot(lightDir, normal), 0.0);
  vec3 diffuse = light.diffuse * diff * Color.rgb;

  // specular
  vec3 reflectDir = reflect(-lightDir, normal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
  vec3 specular = light.specular * (spec * Color.rgb);
  return (ambient + diffuse + specular);
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
  vec3 lightDir = normalize(light.position - fragPos);

  // ambient light
  vec3 ambient = light.ambient * Color.rgb;

  // diffuse light
  float diff = max(dot(lightDir, normal), 0.0);
  vec3 diffuse = light.diffuse * diff * Color.rgb;

  // specular
  vec3 reflectDir = reflect(-lightDir, normal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
  vec3 specular = light.specular * (spec * Color.rgb);

  // attenuation
  float dist = distance(light.position, fragPos);
  float attenuation = 1.0 / (light.constant + light.linear * dist +
                             light.quadratic * dist * dist);
  return (ambient + diffuse + specular) * attenuation;
}

vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
  vec3 lightDir = normalize(light.position - fragPos);

  // ambient light
  vec3 ambient = light.ambient * Color.rgb;

  // diffuse light
  float diff = max(dot(lightDir, normal), 0.0);
  vec3 diffuse = light.diffuse * diff * Color.rgb;

  // specular
  vec3 reflectDir = reflect(-lightDir, normal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
  vec3 specular = light.specular * (spec * Color.rgb);

  float theta = dot(lightDir, normalize(-light.direction));
  float epsilon = light.cutoff - light.outerCutoff;
  float intensity = clamp((theta - light.outerCutoff) / epsilon, 0.0, 1.0);
  return ambient + (diffuse + specular) * intensity;
}
