#version 330 core
// fs.frag - per-pixel shader

in vec2 uvPosition;
out vec4 fragColor;

uniform vec2 iResolution;
uniform float iTime;

void main()
{
	// Normalize coordinates
	vec2 uv = uvPosition / iResolution.xy;
   
	// 
	fragColor = vec4(1.0, 1.0, 1.0, 1.0);
}
