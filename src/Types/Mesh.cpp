#include "Mesh.h"

#include "Registry/ShaderRegistry.h"
#include "Registry/TextureRegistry.h"

#include <fstream>
#include <sstream>
#include <iostream>

mesh_t::mesh_t()
	: m_textureID(0), m_vertex_buffer(0), m_draw_count(0)
{
	LoadOBJ("./pyramid.obj");

	m_textureID = TextureRegistry::GetInstance().GetTexture("test");
}

mesh_t::~mesh_t()
{
	ShaderRegistry::GetInstance().DestroyVertexBuffer(m_vertex_buffer);
}

void mesh_t::Draw()
{
	if (!ShouldDraw())
		return;

	ShaderRegistry &shaders = ShaderRegistry::GetInstance();

	shaders.SetTexture("u_texture", m_textureID);

	shaders.BindVertexBuffer(m_vertex_buffer);
	glDrawArrays(GL_TRIANGLES, 0, (GLsizei)m_draw_count);
	shaders.UnbindVertexBuffer();
}

bool mesh_t::LoadOBJ(const std::string &path)
{
	std::ifstream file(path.c_str());
	if (!file.is_open())
	{
		std::cout << "Failed To Load '" << path.c_str() << "', Using Fallback Cube\n";
		LoadFallbackCube();
		return false;
	}

	std::vector<vec3_t> temp_positions;
	std::vector<vec3_t> temp_normals;
	std::vector<vec2_t> temp_uvs;

	std::vector<unsigned int> vertex_indices;
	std::vector<unsigned int> uv_indices;
	std::vector<unsigned int> normal_indices;

	std::string line;
	while (std::getline(file, line))
	{
		std::istringstream iss(line);
		std::string prefix;
		iss >> prefix;

		if (prefix == "v")
		{
			vec3_t pos;
			iss >> pos.x >> pos.y >> pos.z;
			temp_positions.push_back(pos);
		}
		else if (prefix == "vt")
		{
			vec2_t uv;
			iss >> uv.x >> uv.y;
			temp_uvs.push_back(uv);
		}
		else if (prefix == "vn")
		{
			vec3_t normal;
			iss >> normal.x >> normal.y >> normal.z;
			temp_normals.push_back(normal);
		}
		else if (prefix == "f")
		{
			std::string vertex_str;
			for (int i = 0; i < 3; ++i)
			{
				if (!(iss >> vertex_str))
					break;

				unsigned int pIdx = 0, tIdx = 0, nIdx = 0;
				size_t firstSlash = vertex_str.find('/');
				size_t secondSlash = vertex_str.find('/', firstSlash + 1);

				if (firstSlash == std::string::npos)
				{
					// Format: v
					pIdx = std::atoi(vertex_str.c_str());
				}
				else if (secondSlash == std::string::npos)
				{
					// Format: v/vt
					pIdx = std::atoi(vertex_str.substr(0, firstSlash).c_str());
					tIdx = std::atoi(vertex_str.substr(firstSlash + 1).c_str());
				}
				else
				{
					if (firstSlash + 1 == secondSlash)
					{
						// Format: v//vn
						pIdx = std::atoi(vertex_str.substr(0, firstSlash).c_str());
						nIdx = std::atoi(vertex_str.substr(secondSlash + 1).c_str());
					}
					else
					{
						// Format: v/vt/vn
						pIdx = std::atoi(vertex_str.substr(0, firstSlash).c_str());
						tIdx = std::atoi(vertex_str.substr(firstSlash + 1, secondSlash - firstSlash - 1).c_str());
						nIdx = std::atoi(vertex_str.substr(secondSlash + 1).c_str());
					}
				}

				if (pIdx > 0)
					vertex_indices.push_back(pIdx - 1);
				if (tIdx > 0)
					uv_indices.push_back(tIdx - 1);
				if (nIdx > 0)
					normal_indices.push_back(nIdx - 1);
			}
		}
	}
	file.close();

	if (vertex_indices.empty())
	{
		std::cout << "Failed To Load '" << path.c_str() << "', Using Fallback Cube (NO MESH DATA)\n";
		LoadFallbackCube();
		return false;
	}

	m_verts.clear();
	m_indices.clear();
	m_verts.reserve(vertex_indices.size());

	for (size_t i = 0; i < vertex_indices.size(); ++i)
	{
		vertex_t v;
		unsigned int pIdx = vertex_indices[i];

		if (pIdx < temp_positions.size())
		{
			v.x = temp_positions[pIdx].x;
			v.y = temp_positions[pIdx].y;
			v.z = temp_positions[pIdx].z;
		}

		if (!uv_indices.empty() && i < uv_indices.size() && uv_indices[i] < temp_uvs.size())
		{
			unsigned int tIdx = uv_indices[i];
			v.u = temp_uvs[tIdx].x;
			v.v = temp_uvs[tIdx].y;
		}
		else
		{
			v.u = 0.0f;
			v.v = 0.0f;
		}

		m_verts.push_back(v);
		m_indices.push_back(static_cast<unsigned short>(i));
	}

	ComputeBounds();
	BuildGPUBuffer();

	std::cout << "Loaded OBJ '" << path.c_str() << "' (" << m_verts.size() << " Verts)\n";
	return true;
}

void mesh_t::LoadShader(const std::string &name)
{
	ShaderRegistry::GetInstance().Use(name);
}

bool mesh_t::ShouldDraw()
{
	// !! TODO: Implement Frustum Culling !!
	return true;
}

void mesh_t::LoadFallbackCube()
{
	static const float positions[] = {
		-0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f,
		-0.5f, -0.5f, -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f, -0.5f, -0.5f,
		-0.5f, 0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, -0.5f, -0.5f, 0.5f,
		0.5f, -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 0.5f, -0.5f, 0.5f,
		-0.5f, -0.5f, -0.5f, -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f, -0.5f};

	static const float uvs[] = {
		0.f, 0.f, 1.f, 0.f, 1.f, 1.f, 0.f, 1.f,
		0.f, 0.f, 1.f, 0.f, 1.f, 1.f, 0.f, 1.f,
		0.f, 0.f, 1.f, 0.f, 1.f, 1.f, 0.f, 1.f,
		0.f, 0.f, 1.f, 0.f, 1.f, 1.f, 0.f, 1.f,
		0.f, 0.f, 1.f, 0.f, 1.f, 1.f, 0.f, 1.f,
		0.f, 0.f, 1.f, 0.f, 1.f, 1.f, 0.f, 1.f};

	static const unsigned short indices[] = {
		0, 1, 2, 2, 3, 0,
		4, 5, 6, 6, 7, 4,
		8, 9, 10, 10, 11, 8,
		12, 13, 14, 14, 15, 12,
		16, 17, 18, 18, 19, 16,
		20, 21, 22, 22, 23, 20};

	m_verts.clear();
	m_verts.reserve(24);

	for (int i = 0; i < 24; ++i)
	{
		vertex_t v;
		v.x = positions[i * 3 + 0];
		v.y = positions[i * 3 + 1];
		v.z = positions[i * 3 + 2];

		v.u = uvs[i * 2 + 0];
		v.v = uvs[i * 2 + 1];

		m_verts.push_back(v);
	}

	m_indices.assign(indices, indices + 36);

	ComputeBounds();
	BuildGPUBuffer();
}

void mesh_t::ComputeBounds()
{
	m_bounds = bounds_t();

	for (size_t i = 0; i < m_verts.size(); ++i)
		m_bounds.EncapsulatePoint(m_verts[i]);
}

void mesh_t::BuildGPUBuffer()
{
	ShaderRegistry::GetInstance().DestroyVertexBuffer(m_vertex_buffer);

	if (m_indices.empty())
	{
		m_vertex_buffer = ShaderRegistry::GetInstance().CreateVertexBuffer(m_verts.data(), m_verts.size());
		m_draw_count = (unsigned int)m_verts.size();
		return;
	}

	std::vector<vertex_t> expanded;
	expanded.reserve(m_indices.size());

	for (size_t i = 0; i < m_indices.size(); ++i)
		expanded.push_back(m_verts[m_indices[i]]);

	m_vertex_buffer = ShaderRegistry::GetInstance().CreateVertexBuffer(expanded.data(), expanded.size());
	m_draw_count = (unsigned int)expanded.size();
}