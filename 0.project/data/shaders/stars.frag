#version 330 core

in vec3 FragPos;
in vec2 TexCoords;

out vec4 color;

uniform sampler2D sprite;
uniform vec3 groundBases[3];
uniform vec3 groundUp;
uniform vec3 sunPos;

void main() {
  vec4 s = texture(sprite, TexCoords);
  float sunProj = length(sunPos * groundBases[0] + sunPos * groundBases[1]) / length(sunPos);
  float direction = clamp(dot(sunPos, groundUp) * 100.0, -1.0, 1.0);
  float a = clamp(-sunProj * direction, 0.0, 1.0);
  color = vec4(s.rgb, s.a * a);
}

