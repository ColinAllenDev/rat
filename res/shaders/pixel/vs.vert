#version 330 core
// vs.vert - passes through positions and creates uv coordinates

// Vertex position to pass through to frag shader
layout (location = 0) in vec2 vertexPosition;
// Pixel coordinates
out vec2 uvPosition;
// Screen resolution
uniform vec2 iResolution;

void main() {
	// Pass position through to frag shader
	gl_Position = vec4(vertexPosition, 0.0, 1.0);
	// Convert from clip space (-1 to 1) to pixel coords (0 to resolution)
	uvPosition = (vertexPosition * 0.5 + 0.5) * iResolution;
r
