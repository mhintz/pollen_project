#version 330

in vec4 Color;
in vec3 Normal;
in vec2 TexCoord;

out vec4 oColor;

void main() {
  const vec3 L = vec3(0, 0, 1);
  vec3 N = normalize(Normal);
  float lambert = max(0.0, dot(N, L));
  oColor = Color * vec4(vec3(lambert), 1.0);
}
