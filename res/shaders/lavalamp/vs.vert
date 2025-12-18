#version 330 core

// Input - Vertex positions in clip space
layout (location = 0) in vec2 vertexPosition;
// Output - Vertex positions in pixel space (0 to resolution)
out vec2 fragCoord;
// Uniform - Framebuffer resolution
uniform vec2 iResolution;
// Uniform - Delta time
uniform float iTime;

void main() {
	// Receive vertex position of the frame vertices for fixed 
	// functionality operations on primitives after vertex processing.
	// (i.e primitive assembly, clipping, culling)
	gl_Position = vec4(vertexPosition, 0.0, 1.0);

	// Convert from clip space (-1 to 1) to pixel coords (0 to resolution)
	fragCoord = (vertexPosition * 0.5 + 0.5) * iResolution;
}
