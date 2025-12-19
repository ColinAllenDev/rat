#version 330 core

in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec4 vertexColor;

out vec4 fragColor;
out vec4 fragTexCoord;

uniform mat4 uMvp;

void main() {
    fragTexCoord = vertexTexCoord;
    fragColor = vertexColor;
    gl_Position = uMvp * vec4(vertexPosition, 1.0);
}
