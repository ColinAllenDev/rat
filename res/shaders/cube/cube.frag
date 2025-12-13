#version 330 core

in vec3 vertexColor;  // Received from vertex shader (interpolated across triangle)
out vec4 FragColor;

void main() {
    FragColor = vec4(vertexColor, 1.0);
}
