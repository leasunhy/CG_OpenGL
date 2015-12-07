#version 330 core

#define N_POINT_LIGHT 4

struct Material {
  sampler2D texture_diffuse1;
  sampler2D texture_specular1;
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
in vec4 FragPosLightSpace;

out vec4 FragColor;

uniform DirLight dirLight;
uniform SpotLight spotLight;
uniform PointLight pointLights[N_POINT_LIGHT];
uniform int pointLightCount;

uniform sampler2D shadowMap;

uniform Material material;
uniform vec3 ViewPos;

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
float ShadowCalculation(vec4 fragPosLightSpace, vec3 lightPos);

void main()
{
  vec3 color = texture(material.texture_diffuse1, TexCoords).rgb;
  vec3 normal = normalize(Normal);
  vec3 viewDir = normalize(ViewPos - FragPos);

  vec3 result = vec3(0.0);
  result += calcDirLight(dirLight, normal, viewDir);


  for (int i = 0; i < pointLightCount; ++i){
	result += calcPointLight(pointLights[i], normal, FragPos, viewDir);	
  }
    
  result += calcSpotLight(spotLight, normal, FragPos, viewDir);

  FragColor = vec4(result * color, 1.0f);
}

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir) {
  vec3 lightDir = normalize(-light.direction);

  // ambient
  vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, TexCoords));

  // diffuse
  float diff = max(dot(lightDir, normal), 0.0);
  vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse1, TexCoords));

  // specular
  vec3 reflectDir = reflect(-lightDir, normal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
  vec3 specular = light.specular * (spec * vec3(texture(material.texture_specular1, TexCoords)));
  
  // Calculate shadow
  float shadow = ShadowCalculation(FragPosLightSpace, light.direction);
    shadow = min(shadow, 0.75);
   
  return ambient + (1.0 - shadow) * (diffuse + specular);
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
  vec3 lightDir = normalize(light.position - fragPos);

  // ambient light
  vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, TexCoords));

  // diffuse light
  float diff = max(dot(lightDir, normal), 0.0);
  vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse1, TexCoords));

  // specular
  vec3 reflectDir = reflect(-lightDir, normal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
  vec3 specular = light.specular * (spec * vec3(texture(material.texture_specular1, TexCoords)));

  // attenuation
  float dist = distance(light.position, fragPos);
  float attenuation = 1.0 / (light.constant + light.linear * dist +
                             light.quadratic * dist * dist);

  // Calculate shadow
  float shadow = ShadowCalculation(FragPosLightSpace, light.position);
    shadow = min(shadow, 0.75);

  float dark = 1;
  float radius = sqrt(light.position.y * light.position.y + light.position.x + light.position.x); 
  if (light.position.y <= -0.5f){
	dark = max(light.position.x / radius, 0.05f);
  }
   
  return ambient + (1.0 - shadow) * (diffuse + specular) * dark;
}

vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
  vec3 lightDir = normalize(light.position - fragPos);

  // ambient light
  vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, TexCoords));

  // diffuse light
  float diff = max(dot(lightDir, normal), 0.0);
  vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse1, TexCoords));

  // specular
  vec3 reflectDir = reflect(-lightDir, normal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
  vec3 specular = light.specular * (spec * vec3(texture(material.texture_specular1, TexCoords)));

  float theta = dot(lightDir, normalize(-light.direction));
  float epsilon = light.cutoff - light.outerCutoff;
  float intensity = clamp((theta - light.outerCutoff) / epsilon, 0.0, 1.0);
  
  // Calculate shadow
  float shadow = ShadowCalculation(FragPosLightSpace, light.position);
    shadow = min(shadow, 0.75);
   
  return ambient + (1.0 - shadow) * (diffuse + specular);
}

float ShadowCalculation(vec4 fragPosLightSpace, vec3 lightPos)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // Transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // Get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // Get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // Calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    // Check whether current frag pos is in shadow
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 30.0;
    
    // Keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}
