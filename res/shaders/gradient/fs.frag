#version 330 core
// fs.frag - per-pixel shader

in vec2 fragCoord;
out vec4 FragColor;

uniform vec2 iResolution;
uniform float iTime;

void main() {
	// Normalize coordinates so (0,0) is the middle
	vec2 uv = fragCoord / iResolution.xy;
	// Center coordinates so (0,0) is in the middle
	vec2 centered = uv - 0.5;
	centered.x *= iResolution.x / iResolution.y;
	// Animated background gradient
	vec3 bgColor = vec3(
		0.2 + 0.1 * sin(iTime + uv.x * 3.0),
		0.1 + 0.1 * sin(iTime + 1.3 + uv.y * 3.0),
		0.3 + 0.2 * sin(iTime + 0.7)
	);
	// Draw pulsing circle in the center
	float dist = length(centered);
	float radius = 0.2 + 0.05 * sin(iTime * 2.0);
	float circle = smoothstep(radius + 0.02, radius - 0.02, dist);
	// Circle color shifts over time
    vec3 circleColor = vec3(
        0.5 + 0.5 * cos(iTime),
        0.5 + 0.5 * cos(iTime + 2.094),
        0.5 + 0.5 * cos(iTime + 4.189)
    );
    // Mix background and circle
    FragColor = vec4(mix(bgColor, circleColor, circle), 1.0);
}
