#version 330 core

uniform float u_time;
out vec3 vertexColor;

void main() {
	// Define all 36 vertices of a cube (6 faces × 2 triangles × 3 vertices)
    // Each face is a unit square, centered at the origin, with size 1
    vec3 vertices[36] = vec3[36](
        // Front face (z = +0.5)
        vec3(-0.5, -0.5,  0.5), vec3( 0.5, -0.5,  0.5), vec3( 0.5,  0.5,  0.5),
        vec3(-0.5, -0.5,  0.5), vec3( 0.5,  0.5,  0.5), vec3(-0.5,  0.5,  0.5),
        
        // Back face (z = -0.5)
        vec3( 0.5, -0.5, -0.5), vec3(-0.5, -0.5, -0.5), vec3(-0.5,  0.5, -0.5),
        vec3( 0.5, -0.5, -0.5), vec3(-0.5,  0.5, -0.5), vec3( 0.5,  0.5, -0.5),
        
        // Top face (y = +0.5)
        vec3(-0.5,  0.5,  0.5), vec3( 0.5,  0.5,  0.5), vec3( 0.5,  0.5, -0.5),
        vec3(-0.5,  0.5,  0.5), vec3( 0.5,  0.5, -0.5), vec3(-0.5,  0.5, -0.5),
        
        // Bottom face (y = -0.5)
        vec3(-0.5, -0.5, -0.5), vec3( 0.5, -0.5, -0.5), vec3( 0.5, -0.5,  0.5),
        vec3(-0.5, -0.5, -0.5), vec3( 0.5, -0.5,  0.5), vec3(-0.5, -0.5,  0.5),
        
        // Right face (x = +0.5)
        vec3( 0.5, -0.5,  0.5), vec3( 0.5, -0.5, -0.5), vec3( 0.5,  0.5, -0.5),
        vec3( 0.5, -0.5,  0.5), vec3( 0.5,  0.5, -0.5), vec3( 0.5,  0.5,  0.5),
        
        // Left face (x = -0.5)
        vec3(-0.5, -0.5, -0.5), vec3(-0.5, -0.5,  0.5), vec3(-0.5,  0.5,  0.5),
        vec3(-0.5, -0.5, -0.5), vec3(-0.5,  0.5,  0.5), vec3(-0.5,  0.5, -0.5)
    );

	// Assign a unique color to each face (6 vertices share each color)
    // We'll pass this to the fragment shader
    vec3 faceColors[6] = vec3[6](
        vec3(1.0, 0.65, 0.19),  // Front
        vec3(0.7, 0.45, 0.13),  // Back   
        vec3(1.0, 0.65, 0.25),  // Top   
        vec3(0.7, 0.35, 0.15),  // Bottom
        vec3(0.9, 0.60, 0.19),  // Right
        vec3(0.9, 0.60, 0.19)   // Left
    );

	// Get our vertex position from the array
	vec3 pos = vertices[gl_VertexID];

	// Determine which face this vertex belongs to (6 verts per face)
	int faceIndex = gl_VertexID / 6;
	vertexColor = faceColors[faceIndex];

	// == ROTATION ==
	// We'll rotate around both Y and X axes for a nice tumbling effect
    float angleY = u_time * 0.7;  // Rotate around Y axis
    float angleX = u_time * 0.5;  // Rotate around X axis
    
    // Rotation matrix around Y axis
    mat3 rotY = mat3(
         cos(angleY), 0.0, sin(angleY),
         0.0,         1.0, 0.0,
        -sin(angleY), 0.0, cos(angleY)
    );
    
    // Rotation matrix around X axis
    mat3 rotX = mat3(
        1.0, 0.0,          0.0,
        0.0, cos(angleX), -sin(angleX),
        0.0, sin(angleX),  cos(angleX)
    );	

	// Apply rotations (order matters: first Y, then X)
    pos = rotX * rotY * pos;

	// == VIEW TRANSFORM == //
	// Move the cube back from the camera
	vec3 viewPos = pos;
	viewPos.z -= 2.5;
    
    // === PROJECTION ===
	float fov = 1.5;	// Field of view factor
	float near = 0.5;	// Near clipping
	float far = 10.0; 	// Far clipping

	// These coefficients map view-space z to clip-space z correctly
	// After perspective divide, z values will be in [1, -1] range
	// with closer objets have smaller values (for GL_LESS depth test)
	float A = -(far + near) / (far - near);
	float B = -2.0 * far * near / (far - near);
    
    // Build the clip-space position using homogeneous coordinates
    // The GPU will automatically divide xyz by w (the "perspective divide")
    gl_Position = vec4(
        viewPos.x * fov,      // x: scaled by FOV, will be divided by w
        viewPos.y * fov,      // y: scaled by FOV, will be divided by w  
        A * viewPos.z + B,    // z: properly mapped depth value
        -viewPos.z            // w: the perspective divisor (positive, since viewPos.z is negative)
    );
}
