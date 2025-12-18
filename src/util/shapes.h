#ifndef SHAPES_H
#define SHAPES_H

typedef struct rt_mesh {
	int indices;
	float vertices[];
} rt_mesh;

const rt_mesh m_cube = {
	.vertices = {
		 1,  1,  1,
		-1,  1,  1,
		 1, -1,  1,
		-1, -1,  1,

		 1,  1, -1,
		 1, -1, -1,
		-1,  1, -1,
		-1, -1, -1,
	},
	.indices = 4
};

#endif /* SHAPES_H */
