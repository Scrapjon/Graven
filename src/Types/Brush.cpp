#include "Brush.h"

#include "Registry/ShaderRegistry.h"
#include "Registry/TextureRegistry.h"
#include "World.h"

#include <cmath>
#include <limits>

namespace
{
	const float kBrushEpsilon = 0.03125f;
	const float kDebugNormalLength = 12.f;

	// Below This Length An Axis Is Degenerate (Parallel Edges) - Contributes No Separation Info
	const float kAxisEpsilon = 1e-6f;

	// Returns False Only If No Overlap
	bool AxisOverlapInterval(const vec3_t &axis, const vec3_t &box_center0, const vec3_t &half_extents, const vec3_t &d, const vec3_t &p0, const vec3_t &p1, const vec3_t &p2, float &t_enter, float &t_exit)
	{
		const float axis_len = vec3_t::Length(axis);
		if (axis_len < kAxisEpsilon)
		{
			t_enter = -std::numeric_limits<float>::max();
			t_exit = std::numeric_limits<float>::max();
			return true;
		}

		const vec3_t a = axis * (1.f / axis_len);

		const float tp0 = vec3_t::Dot(p0, a);
		const float tp1 = vec3_t::Dot(p1, a);
		const float tp2 = vec3_t::Dot(p2, a);
		const float tri_min = std::min(tp0, std::min(tp1, tp2));
		const float tri_max = std::max(tp0, std::max(tp1, tp2));

		const float center_proj = vec3_t::Dot(box_center0, a);
		const float radius = std::fabs(half_extents.x * a.x) + std::fabs(half_extents.y * a.y) + std::fabs(half_extents.z * a.z);

		const float box_min0 = center_proj - radius;
		const float box_max0 = center_proj + radius;

		const float v = vec3_t::Dot(d, a); // Rate Of Change Of The Box's Projection Per Unit t

		if (std::fabs(v) < kAxisEpsilon)
		{
			// Box Isn't Moving Along This Axis
			if (box_max0 < tri_min || box_min0 > tri_max)
			{
				t_enter = std::numeric_limits<float>::max();
				t_exit = -std::numeric_limits<float>::max();
				return false;
			}

			t_enter = -std::numeric_limits<float>::max();
			t_exit = std::numeric_limits<float>::max();
			return true;
		}

		const float t_a = (tri_min - box_max0) / v;
		const float t_b = (tri_max - box_min0) / v;

		if (t_a < t_b)
		{
			t_enter = t_a;
			t_exit = t_b;
		}
		else
		{
			t_enter = t_b;
			t_exit = t_a;
		}

		return true;
	}

	bool SweepBoxTriangle(const vec3_t &box_center0, const vec3_t &half_extents, const vec3_t &d,
						  const vec3_t &p0, const vec3_t &p1, const vec3_t &p2,
						  float max_fraction, float &out_fraction, vec3_t &out_normal)
	{
		const vec3_t box_axes[3] = {vec3_t(1.f, 0.f, 0.f), vec3_t(0.f, 1.f, 0.f), vec3_t(0.f, 0.f, 1.f)};
		const vec3_t tri_edges[3] = {p1 - p0, p2 - p1, p0 - p2};
		const vec3_t tri_normal = vec3_t::Cross(tri_edges[0], tri_edges[1]);

		vec3_t axes[13];
		int axis_count = 0;

		axes[axis_count++] = box_axes[0];
		axes[axis_count++] = box_axes[1];
		axes[axis_count++] = box_axes[2];
		axes[axis_count++] = tri_normal;

		for (int i = 0; i < 3; ++i)
			for (int j = 0; j < 3; ++j)
				axes[axis_count++] = vec3_t::Cross(box_axes[i], tri_edges[j]);

		float global_enter = -std::numeric_limits<float>::max();
		float global_exit = std::numeric_limits<float>::max();
		vec3_t hit_axis(0.f);

		for (int i = 0; i < axis_count; ++i)
		{
			float t_enter, t_exit;
			if (!AxisOverlapInterval(axes[i], box_center0, half_extents, d, p0, p1, p2, t_enter, t_exit)) // Found Separating Axis So No Collision
				return false;

			if (t_enter > global_enter)
			{
				global_enter = t_enter;
				hit_axis = axes[i];
			}

			if (t_exit < global_exit)
				global_exit = t_exit;

			if (global_enter > global_exit) // Intervals Don't Overlap Across Axes So No Collision
				return false;
		}

		if (global_enter > max_fraction || global_exit < 0.f)
			return false;

		const float fraction = (global_enter > 0.f) ? global_enter : 0.f;
		if (fraction > max_fraction)
			return false;

		const float hit_axis_len = vec3_t::Length(hit_axis);
		vec3_t normal = (hit_axis_len > kAxisEpsilon) ? hit_axis * (1.f / hit_axis_len) : vec3_t(0.f, 1.f, 0.f);

		// Get Normal Of Collision
		const vec3_t box_center_at_hit = box_center0 + d * fraction;
		if (vec3_t::Dot(normal, box_center_at_hit - p0) < 0.f)
			normal = -normal;

		out_fraction = fraction;
		out_normal = normal;
		return true;
	}
} // namespace

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

		batch.use_wavy = !face.texture.empty() && face.texture.find("&") != std::string::npos;

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

	bool wavy_active = false;

	for (size_t i = 0; i < m_batches.size(); ++i)
	{
		const brush_batch_t &batch = m_batches[i];

		if (batch.texture_name == "__TB_empty")
			continue;

		if (batch.use_wavy != wavy_active)
		{
			wavy_active = batch.use_wavy;

			shaders.Use(wavy_active ? "wavy" : "default");

			if (wavy_active)
			{
				shaders.SetMatrix("u_mvp", World::GetActiveCamera()->GetViewProjectionMatrix());
				shaders.SetFloat("u_time", (float)World::GetTimer() / 60.f);
			}
		}

		GLuint texture = textures.GetTexture(batch.texture_name);
		if (!texture)
			texture = textures.GetTexture("checker");

		shaders.SetTexture("u_texture", texture);
		glDrawArrays(GL_TRIANGLES, batch.offset, batch.count);
	}

	// Restore The Default Shader
	if (wavy_active)
		shaders.Use("default");

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

	const vec3_t half_extents = (maxs - mins) * 0.5f;
	const vec3_t box_center0 = start + (mins + maxs) * 0.5f;
	const vec3_t displacement = end - start;

	if (vec3_t::Length(displacement) < 0.0001f)
		return false;

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

			float fraction;
			vec3_t normal;
			if (SweepBoxTriangle(box_center0, half_extents, displacement, p0, p1, p2, trace.fraction, fraction, normal))
			{
				if (fraction < trace.fraction)
				{
					trace.fraction = fraction;
					trace.normal = normal;
					trace.end_pos = start + displacement * fraction;
					trace.in_open = false;
					hit = true;
				}
			}
		}
	}

	return hit;
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