#ifndef MESH_H
#define MESH_H

#include <GL/gl.h>
#include <string>
#include <vector>

#include "Bounds.h"
#include "Vertex.h"

typedef struct mesh_t
{

public:
	mesh_t();
	~mesh_t();

	void Draw();

	// Load Tris From .OBJ File
	bool LoadOBJ(const std::string &path);

	// Load Shader To Use When Drawing Mesh
	void LoadShader(const std::string &name);

private:
	bounds_t m_bounds;
	vec3_t m_pos;
	GLuint m_textureID;
	GLuint m_vertex_buffer;
	std::vector<vertex_t> m_verts;
	std::vector<unsigned short> m_indices;

	unsigned int m_draw_count;

	// Whether The Bounds Is In Range
	bool ShouldDraw();

	// If Mesh Is Not Found In LoadOBJ, Use This Hardcoded Cube
	void LoadFallbackCube();

	// Compute Vert Boundaries For Culling Later
	void ComputeBounds();

	void BuildGPUBuffer();

} mesh_t;

#endif /* MESH_H */