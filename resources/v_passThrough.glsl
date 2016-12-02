#version 330

in vec4 ciPosition;
in vec4 ciColor;
in vec3 ciNormal;
in vec2 ciTexCoord0;

uniform mat4 ciModelViewProjection;
uniform mat3 ciNormalMatrix;

out lowp vec4 Color;
out highp vec3 Normal;
out highp vec2 TexCoord;

void main() {
  gl_Position = ciModelViewProjection * ciPosition;
  TexCoord = ciTexCoord0;
  Color = ciColor;
  Normal = ciNormalMatrix * ciNormal;
}
