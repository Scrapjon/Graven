#ifndef BRUSH_H
#define BRUSH_H

#include <string>
#include <vector>

#include "Bounds.h"
#include "Trace.h"
#include "Vector.h"
#include "Vertex.h"

#include <GL/gl.h>

typedef struct
{
	std::string texture_name;
	GLsizei offset;
	GLsizei count;
} brush_batch_t;

typedef struct
{
	std::string texture;
	std::vector<vertex_t> verts;
} brush_face_t;

class brush_t
{
public:
	brush_t();
	~brush_t();

	void AddFace(const brush_face_t &face) { m_faces.push_back(face); }

	void BuildRenderData();

	void Draw();

	bool ContainsPoint(const vec3_t &point) const;
	bool OverlapsBounds(const bounds_t &bounds) const;
	bool TraceBox(const vec3_t &start, const vec3_t &end, const vec3_t &mins, const vec3_t &maxs, trace_t &trace) const;

	const std::vector<brush_face_t> &GetFaces() const { return m_faces; }
	const bounds_t &GetBounds() const { return m_bounds; }

private:
	std::vector<brush_face_t> m_faces;
	std::vector<brush_batch_t> m_batches;
	bounds_t m_bounds;

	unsigned int m_vertex_buffer;
	unsigned int m_draw_count;

	void ComputeBounds();

	brush_t(const brush_t &);
	brush_t &operator=(const brush_t &);
};

#endif /* BRUSH_H */