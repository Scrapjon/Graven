#include "Brush.h"

#include "Registry/ShaderRegistry.h"
#include "Registry/TextureRegistry.h"

#include <cmath>

namespace
{
	const float kBrushEpsilon = 0.03125f;
	const float kDebugNormalLength = 12.f;
}

brush_t::brush_t()
	: m_vertex_buffer(0), m_draw_count(0)
{
}

brush_t::~brush_t()
{
	if (m_vertex_buffer)
		ShaderRegistry::GetInstance().DestroyVertexBuffer((GLuint)m_vertex_buffer);
}

void brush_t::ComputeBounds()
{
	m_bounds = bounds_t();

	bool any = false;
	for (size_t f = 0; f < m_faces.size(); ++f)
	{
		for (size_t v = 0; v < m_faces[f].verts.size(); ++v)
		{
			m_bounds.EncapsulatePoint(m_faces[f].verts[v]);
			any = true;
		}
	}

	if (!any)
		m_bounds = bounds_t(vec3_t(0.f), vec3_t(0.f));
}

void brush_t::BuildRenderData()
{
	ComputeBounds();

	std::vector<vertex_t> all_verts;
	m_batches.clear();

	GLsizei current_offset = 0;
	for (size_t f = 0; f < m_faces.size(); ++f)
	{
		const brush_face_t &face = m_faces[f];
		if (face.verts.empty())
			continue;

		brush_batch_t batch;
		batch.texture_name = face.texture;
		batch.offset = current_offset;
		batch.count = (GLsizei)face.verts.size();
		m_batches.push_back(batch);

		all_verts.insert(all_verts.end(), face.verts.begin(), face.verts.end());
		current_offset += (GLsizei)face.verts.size();
	}

	if (all_verts.empty())
		return;

	if (m_vertex_buffer)
		ShaderRegistry::GetInstance().DestroyVertexBuffer((GLuint)m_vertex_buffer);

	m_vertex_buffer = ShaderRegistry::GetInstance().CreateVertexBuffer(all_verts.data(), all_verts.size());
	m_draw_count = (unsigned int)all_verts.size();
}

void brush_t::Draw()
{
	if (m_batches.empty() || !m_vertex_buffer)
		return;

	ShaderRegistry &shaders = ShaderRegistry::GetInstance();
	TextureRegistry &textures = TextureRegistry::GetInstance();

	shaders.BindVertexBuffer((GLuint)m_vertex_buffer);

	for (size_t i = 0; i < m_batches.size(); ++i)
	{
		const brush_batch_t &batch = m_batches[i];

		if (batch.texture_name == "__TB_empty")
			continue;

		GLuint texture = textures.GetTexture(batch.texture_name);
		if (!texture)
			texture = textures.GetTexture("checker");

		shaders.SetTexture("u_texture", texture);
		glDrawArrays(GL_TRIANGLES, batch.offset, batch.count);
	}

	shaders.UnbindVertexBuffer();
}

bool brush_t::ContainsPoint(const vec3_t &point) const
{
	if (m_faces.empty())
		return false;

	vec3_t ray_dir(1.0f, 0.37f, 0.13f); // Arbitrary Non Axis Aligned Dir To (Hopefully) Ensure Collision
	int intersections = 0;

	for (size_t f = 0; f < m_faces.size(); ++f)
	{
		const std::vector<vertex_t> &verts = m_faces[f].verts;
		if (verts.size() < 3)
			continue;

		for (size_t t = 1; t + 1 < verts.size(); ++t)
		{
			const vec3_t &p0 = verts[0];
			const vec3_t &p1 = verts[t];
			const vec3_t &p2 = verts[t + 1];

			// Moller Trumbore
			vec3_t edge1 = p1 - p0;
			vec3_t edge2 = p2 - p0;
			vec3_t h = vec3_t::Cross(ray_dir, edge2);
			float a = vec3_t::Dot(edge1, h);

			if (std::fabs(a) < 1e-6) // Parallel
				continue;

			float f_inv = 1.0f / a;
			vec3_t s = point - p0;
			float u = f_inv * vec3_t::Dot(s, h);

			if (u < 0.0f || u > 1.0f)
				continue;

			vec3_t q = vec3_t::Cross(s, edge1);
			float v = f_inv * vec3_t::Dot(ray_dir, q);

			if (v < 0.0f || u + v > 1.0f)
				continue;

			float t_hit = f_inv * vec3_t::Dot(edge2, q);
			if (t_hit > 1e-5f)
			{
				intersections++;
			}
		}
	}

	return (intersections % 2) == 1;
}

bool brush_t::TraceBox(const vec3_t &start, const vec3_t &end, const vec3_t &mins, const vec3_t &maxs, trace_t &trace) const
{
	if (m_faces.empty())
		return false;

	vec3_t sweep_min = start + mins;
	vec3_t sweep_max = start + maxs;
	sweep_min.x = std::min(sweep_min.x, end.x + mins.x);
	sweep_min.y = std::min(sweep_min.y, end.y + mins.y);
	sweep_min.z = std::min(sweep_min.z, end.z + mins.z);
	sweep_max.x = std::max(sweep_max.x, end.x + maxs.x);
	sweep_max.y = std::max(sweep_max.y, end.y + maxs.y);
	sweep_max.z = std::max(sweep_max.z, end.z + maxs.z);

	if (sweep_max.x < m_bounds.min.x || sweep_min.x > m_bounds.max.x ||
		sweep_max.y < m_bounds.min.y || sweep_min.y > m_bounds.max.y ||
		sweep_max.z < m_bounds.min.z || sweep_min.z > m_bounds.max.z)
	{
		return false;
	}

	if (ContainsPoint(start))
	{
		trace.all_solid = true;
		trace.start_solid = true;
		trace.fraction = 0.0f;
		trace.end_pos = start;
		return true;
	}

	vec3_t ray = end - start;
	float ray_len = vec3_t::Length(ray);
	if (ray_len < 0.0001f)
		return false;

	vec3_t ray_dir = ray * (1.0f / ray_len);

	// Boundint Box Corners
	const vec3_t corners[8] = {
		vec3_t(mins.x, mins.y, mins.z),
		vec3_t(maxs.x, mins.y, mins.z),
		vec3_t(mins.x, maxs.y, mins.z),
		vec3_t(maxs.x, maxs.y, mins.z),
		vec3_t(mins.x, mins.y, maxs.z),
		vec3_t(maxs.x, mins.y, maxs.z),
		vec3_t(mins.x, maxs.y, maxs.z),
		vec3_t(maxs.x, maxs.y, maxs.z)};

	float earliest_impact = 1.0f;
	vec3_t hit_normal(0.0f);
	bool hit = false;

	for (size_t f = 0; f < m_faces.size(); ++f)
	{
		const std::vector<vertex_t> &verts = m_faces[f].verts;
		if (verts.size() < 3)
			continue;

		for (size_t t = 1; t + 1 < verts.size(); ++t)
		{
			const vec3_t &p0 = verts[0];
			const vec3_t &p1 = verts[t];
			const vec3_t &p2 = verts[t + 1];

			vec3_t edge1 = p1 - p0;
			vec3_t edge2 = p2 - p0;
			vec3_t tri_normal = vec3_t::Normalize(vec3_t::Cross(edge1, edge2));

			for (int i = 0; i < 8; ++i)
			{
				vec3_t corner_start = start + corners[i];
				vec3_t s = corner_start - p0;

				vec3_t h = vec3_t::Cross(ray_dir, edge2);
				float a = vec3_t::Dot(edge1, h);

				if (std::fabs(a) < 1e-6f)
					continue;

				float f_inv = 1.0f / a;
				float u = f_inv * vec3_t::Dot(s, h);

				if (u < 0.0f || u > 1.0f)
					continue;

				vec3_t q = vec3_t::Cross(s, edge1);
				float v = f_inv * vec3_t::Dot(ray_dir, q);

				if (v < 0.0f || u + v > 1.0f)
					continue;

				float t_hit = f_inv * vec3_t::Dot(edge2, q);
				if (t_hit >= 0.0f && t_hit <= ray_len)
				{
					float fraction = t_hit / ray_len;
					if (fraction < earliest_impact && fraction < trace.fraction)
					{
						earliest_impact = fraction;
						hit_normal = tri_normal;
						hit = true;
					}
				}
			}
		}
	}

	if (hit)
	{
		trace.fraction = earliest_impact;
		trace.normal = hit_normal;
		trace.end_pos = start + ray * earliest_impact;
		trace.in_open = false;
		return true;
	}

	return false;
}

bool brush_t::OverlapsBounds(const bounds_t &bounds) const
{
	if (bounds.max.x < m_bounds.min.x || bounds.min.x > m_bounds.max.x)
		return false;
	if (bounds.max.y < m_bounds.min.y || bounds.min.y > m_bounds.max.y)
		return false;
	if (bounds.max.z < m_bounds.min.z || bounds.min.z > m_bounds.max.z)
		return false;
	return true;
}
