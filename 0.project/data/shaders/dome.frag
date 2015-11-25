#version 330 core

in vec3 FragPos;

out vec4 color;

uniform vec3 sunPos;
uniform sampler2D glow;
uniform sampler2D domeColor;

void main()
{
  vec3 fp = normalize(FragPos);
  vec3 sp = normalize(sunPos);
  float proximity = dot(fp, sp);
  vec4 c = texture(domeColor, vec2((sp.y + 1.0) / 2.0, fp.y));
  //vec4 c = texture(domeColor, vec2(0.5, 1.0 - fp.y));
  vec4 g = texture(glow, vec2((sp.y + 1.0) / 2.0, proximity));
  color = vec4(c.rgb + g.rgb * g.a / 2.0, c.a);
  //color = mix(c, g, 0.2);
}
