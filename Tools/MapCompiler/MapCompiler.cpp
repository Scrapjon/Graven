#include "Types/Vector.h"
#include "Plane.h"
#include "Types/Vertex.h"

#include <cmath>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace
{
	const float kClipEpsilon = 0.001f;

	struct map_face_t
	{
		vec3_t p0, p1, p2;
		std::string texture;
		float xoff, yoff, rotation, xscale, yscale;

		bool is_valve220;
		vec3_t u_axis, v_axis;

		map_face_t() : xoff(0.f), yoff(0.f), rotation(0.f), xscale(1.f), yscale(1.f), is_valve220(false) {}
	};

	struct map_brush_t
	{
		std::vector<map_face_t> faces;
	};

	struct map_entity_t
	{
		std::string classname;
		std::map<std::string, std::string> keyvalues;
		std::vector<map_brush_t> brushes;
	};

	// Output Data For brush_t
	struct compiled_face_t
	{
		std::string texture;
		std::vector<vertex_t> verts;
	};

	struct compiled_brush_t
	{
		std::vector<compiled_face_t> faces;
	};

	// Helpers
	std::string Trim(const std::string &s)
	{
		const size_t start = s.find_first_not_of(" \t");
		if (start == std::string::npos)
			return "";
		const size_t end = s.find_last_not_of(" \t");
		return s.substr(start, end - start + 1);
	}

	std::string ReadQuoted(std::istream &in)
	{
		std::string result;
		char c = 0;

		in >> std::ws;
		if (!in.get(c) || c != '"')
			return result;

		while (in.get(c) && c != '"')
			result += c;

		return result;
	}

	vec3_t ParseVec3(const std::string &s)
	{
		std::istringstream iss(s);
		vec3_t v;
		iss >> v.x >> v.y >> v.z;
		return v;
	}

	std::string ReplaceExtension(const std::string &path, const std::string &new_ext)
	{
		const size_t dot = path.find_last_of('.');
		const size_t slash = path.find_last_of("/\\");
		std::string base = (dot != std::string::npos && (slash == std::string::npos || dot > slash))
							   ? path.substr(0, dot)
							   : path;
		return base + new_ext;
	}

	// Convert Trenchbroom Z Up To Y Up
	vec3_t ToYUp(const vec3_t &v)
	{
		return vec3_t(v.x, v.z, -v.y);
	}

	// Binary Write Helpers
	template <typename T>
	void WriteRaw(std::ofstream &out, const T &value)
	{
		out.write(reinterpret_cast<const char *>(&value), sizeof(T));
	}

	void WriteString(std::ofstream &out, const std::string &s)
	{
		WriteRaw(out, (int)s.size());
		if (!s.empty())
			out.write(s.data(), (std::streamsize)s.size());
	}

	// Parse Lines Like:
	// ( -88 64 -8 ) ( -88 64 -7 ) ( -88 65 -8 ) __TB_empty [ 0 1 0 0 ] [ 0 0 -1 0 ] 0 1 1
	// Or
	// ( -88 64 -8 ) ( -88 64 -7 ) ( -88 65 -8 ) __TB_empty 0 0 0 1 1
	bool ParseFaceLine(const std::string &line, map_face_t &face)
	{
		std::istringstream iss(line);
		char paren;

		iss >> paren >> face.p0.x >> face.p0.y >> face.p0.z >> paren;
		iss >> paren >> face.p1.x >> face.p1.y >> face.p1.z >> paren;
		iss >> paren >> face.p2.x >> face.p2.y >> face.p2.z >> paren;

		if (iss.fail())
			return false;

		iss >> face.texture;
		if (iss.fail())
			return false;

		// Check If Valve Or Standard Format
		iss >> std::ws;
		char next_char = iss.peek();

		if (next_char == '[')
		{
			face.is_valve220 = true;
			char b;
			float ux, uy, uz, uoff;
			float vx, vy, vz, voff;

			// Parse U Vector And Offset
			iss >> b >> ux >> uy >> uz >> uoff >> b;
			// Parse V Vector And Offset
			iss >> b >> vx >> vy >> vz >> voff >> b;

			face.u_axis = vec3_t(ux, uy, uz);
			face.v_axis = vec3_t(vx, vy, vz);
			face.xoff = uoff;
			face.yoff = voff;

			iss >> face.rotation >> face.xscale >> face.yscale;
		}
		else
		{
			face.is_valve220 = false;
			iss >> face.xoff >> face.yoff >> face.rotation >> face.xscale >> face.yscale;
		}

		return !iss.fail();
	}

	bool ParseKeyValueLine(const std::string &line, std::string &key, std::string &value)
	{
		std::istringstream iss(line);
		key = ReadQuoted(iss);
		value = ReadQuoted(iss);
		return !key.empty();
	}

	bool ParseMapFile(const std::string &path, std::vector<map_brush_t> &out_brushes, std::vector<map_entity_t> &out_entities)
	{
		std::ifstream file(path.c_str());

		if (!file.is_open())
		{
			std::cout << "AAAAAAAAAAAAAAAAAA\n";
			return false;
		}

		std::string line;
		int depth = 0;
		map_entity_t current_entity;
		map_brush_t current_brush;

		while (std::getline(file, line))
		{
			while (!line.empty() && (line[line.size() - 1] == '\r' || line[line.size() - 1] == '\n'))
				line.erase(line.size() - 1);

			const std::string trimmed = Trim(line);
			if (trimmed.empty() || trimmed.compare(0, 2, "//") == 0)
				continue;

			if (trimmed == "{")
			{
				++depth;
				if (depth == 1)
					current_entity = map_entity_t();
				else if (depth == 2)
					current_brush = map_brush_t();
				continue;
			}

			if (trimmed == "}")
			{
				if (depth == 2)
				{
					out_brushes.push_back(current_brush);
					current_entity.brushes.push_back(current_brush);
				}
				else if (depth == 1)
				{
					out_entities.push_back(current_entity);
				}
				--depth;
				continue;
			}

			if (depth == 2 && trimmed[0] == '(')
			{
				map_face_t face;
				if (ParseFaceLine(trimmed, face))
					current_brush.faces.push_back(face);
				continue;
			}

			if (depth == 1 && trimmed[0] == '"')
			{
				std::string key, value;
				if (ParseKeyValueLine(trimmed, key, value))
				{
					if (key == "classname")
						current_entity.classname = value;
					current_entity.keyvalues[key] = value;
				}
				continue;
			}
		}

		return true;
	}

	void TextureAxisFromPlane(const vec3_t &normal, vec3_t &xv, vec3_t &yv)
	{
		static const vec3_t baseaxis[18] = {
			vec3_t(0, 0, 1),
			vec3_t(1, 0, 0),
			vec3_t(0, -1, 0),
			vec3_t(0, 0, -1),
			vec3_t(1, 0, 0),
			vec3_t(0, -1, 0),
			vec3_t(1, 0, 0),
			vec3_t(0, 1, 0),
			vec3_t(0, 0, -1),
			vec3_t(-1, 0, 0),
			vec3_t(0, 1, 0),
			vec3_t(0, 0, -1),
			vec3_t(0, 1, 0),
			vec3_t(1, 0, 0),
			vec3_t(0, 0, -1),
			vec3_t(0, -1, 0),
			vec3_t(1, 0, 0),
			vec3_t(0, 0, -1),
		};

		int best_axis = 0;
		float best_dot = 0.f;

		for (int i = 0; i < 6; ++i)
		{
			const float d = vec3_t::Dot(normal, baseaxis[i * 3]);
			if (d > best_dot)
			{
				best_dot = d;
				best_axis = i;
			}
		}

		xv = baseaxis[best_axis * 3 + 1];
		yv = baseaxis[best_axis * 3 + 2];
	}

	void ComputeUV(const vec3_t &point_zup, const vec3_t &normal_zup, const map_face_t &f, float &u, float &v)
	{
		const float sx = (f.xscale != 0.f) ? f.xscale : 1.f;
		const float sy = (f.yscale != 0.f) ? f.yscale : 1.f;

		if (f.is_valve220)
		{
			u = ((vec3_t::Dot(point_zup, f.u_axis) / sx) + f.xoff) / 256.f;
			v = ((vec3_t::Dot(point_zup, f.v_axis) / sy) + f.yoff) / -256.f;
		}
		else
		{
			vec3_t xv, yv;
			TextureAxisFromPlane(normal_zup, xv, yv);

			const float rad = f.rotation * 3.14159265f / 180.f;
			const float c = std::cos(rad);
			const float s = std::sin(rad);

			const vec3_t rxv = (xv * c) - (yv * s);
			const vec3_t ryv = (xv * s) + (yv * c);

			u = ((vec3_t::Dot(point_zup, rxv) / sx) + f.xoff) / 256.f;
			v = ((vec3_t::Dot(point_zup, ryv) / sy) - f.yoff) / -256.f;
		}
	}

	std::vector<vec3_t> MakeHugePolygon(const plane_t &plane)
	{
		const float size = 8192.f;

		vec3_t up = (std::fabs(plane.normal.z) < 0.99f) ? vec3_t(0.f, 0.f, 1.f) : vec3_t(1.f, 0.f, 0.f);
		vec3_t right = vec3_t::Normalize(vec3_t::Cross(up, plane.normal));
		up = vec3_t::Normalize(vec3_t::Cross(plane.normal, right));

		const vec3_t origin = plane.normal * plane.dist;

		std::vector<vec3_t> poly;
		poly.push_back(origin + (right * -size) + (up * -size));
		poly.push_back(origin + (right * size) + (up * -size));
		poly.push_back(origin + (right * size) + (up * size));
		poly.push_back(origin + (right * -size) + (up * size));
		return poly;
	}

	std::vector<vec3_t> ClipPolygon(const std::vector<vec3_t> &poly, const plane_t &plane)
	{
		std::vector<vec3_t> result;
		if (poly.empty())
			return result;

		for (size_t i = 0; i < poly.size(); ++i)
		{
			const vec3_t &curr = poly[i];
			const vec3_t &prev = poly[(i + poly.size() - 1) % poly.size()];

			const float curr_dist = plane.DistanceTo(curr);
			const float prev_dist = plane.DistanceTo(prev);

			const bool curr_inside = curr_dist <= kClipEpsilon;
			const bool prev_inside = prev_dist <= kClipEpsilon;

			if (curr_inside != prev_inside)
			{
				const float t = prev_dist / (prev_dist - curr_dist);
				result.push_back(prev + (curr - prev) * t);
			}

			if (curr_inside)
				result.push_back(curr);
		}

		return result;
	}

	compiled_brush_t CompileBrush(const map_brush_t &brush)
	{
		compiled_brush_t out;

		std::vector<plane_t> planes_zup;
		for (size_t i = 0; i < brush.faces.size(); ++i)
		{
			const map_face_t &f = brush.faces[i];
			planes_zup.push_back(plane_t::FromPoints(f.p0, f.p1, f.p2));
		}

		for (size_t i = 0; i < brush.faces.size(); ++i)
		{
			const map_face_t &f = brush.faces[i];
			const plane_t &plane = planes_zup[i];

			std::vector<vec3_t> poly = MakeHugePolygon(plane);
			for (size_t j = 0; j < planes_zup.size() && !poly.empty(); ++j)
			{
				if (j == i)
					continue;
				poly = ClipPolygon(poly, planes_zup[j]);
			}

			if (poly.size() < 3) // Non Existent Poly
				continue;

			compiled_face_t face;
			face.texture = f.texture;

			for (size_t t = 1; t + 1 < poly.size(); ++t)
			{
				const vec3_t tri[3] = {poly[0], poly[t], poly[t + 1]};
				for (int k = 0; k < 3; ++k)
				{
					float u, v;
					ComputeUV(tri[k], plane.normal, f, u, v);

					const vec3_t converted = ToYUp(tri[k]);

					vertex_t vert;
					vert.x = converted.x;
					vert.y = converted.y;
					vert.z = converted.z;
					vert.u = u;
					vert.v = v;
					face.verts.push_back(vert);
				}
			}

			out.faces.push_back(face);
		}

		return out;
	}

	int CountNonClassnameKeys(const map_entity_t &e)
	{
		int count = 0;
		for (std::map<std::string, std::string>::const_iterator it = e.keyvalues.begin(); it != e.keyvalues.end(); ++it)
		{
			if (it->first != "classname")
				++count;
		}
		return count;
	}

	bool WriteCompiledMap(const std::string &path, const std::vector<map_brush_t> &brushes, const std::vector<map_entity_t> &entities)
	{
		std::ofstream out(path.c_str(), std::ios::binary);
		if (!out.is_open())
			return false;

		std::vector<compiled_brush_t> compiled;
		compiled.reserve(brushes.size());
		for (size_t i = 0; i < brushes.size(); ++i)
			compiled.push_back(CompileBrush(brushes[i]));

		out.write("GMPB", 4);
		WriteRaw(out, (int)compiled.size());

		for (size_t i = 0; i < compiled.size(); ++i)
		{
			const compiled_brush_t &b = compiled[i];
			WriteRaw(out, (int)b.faces.size());

			for (size_t f = 0; f < b.faces.size(); ++f)
			{
				const compiled_face_t &face = b.faces[f];
				WriteString(out, face.texture);
				WriteRaw(out, (int)face.verts.size());

				if (!face.verts.empty())
					out.write(reinterpret_cast<const char *>(face.verts.data()), face.verts.size() * sizeof(vertex_t));
			}
		}

		std::vector<const map_entity_t *> point_entities;
		for (size_t i = 0; i < entities.size(); ++i)
		{
			if (entities[i].brushes.empty())
				point_entities.push_back(&entities[i]);
		}

		WriteRaw(out, (int)point_entities.size());

		for (size_t i = 0; i < point_entities.size(); ++i)
		{
			const map_entity_t &e = *point_entities[i];
			WriteString(out, e.classname);
			WriteRaw(out, CountNonClassnameKeys(e));

			for (std::map<std::string, std::string>::const_iterator it = e.keyvalues.begin(); it != e.keyvalues.end(); ++it)
			{
				if (it->first == "classname")
					continue;

				WriteString(out, it->first);

				if (it->first == "origin")
				{
					const vec3_t converted = ToYUp(ParseVec3(it->second));
					std::ostringstream oss;
					oss << converted.x << " " << converted.y << " " << converted.z;
					WriteString(out, oss.str());
				}
				else
				{
					WriteString(out, it->second);
				}
			}
		}

		return true;
	}
} // namespace

int main(int argc, char **argv)
{
	if (argc < 2)
	{
		std::cerr << "Usage: mapc <input.map> [output.gmap]\n";
		return 1;
	}

	const std::string input_path = argv[1];
	const std::string output_path = (argc >= 3) ? argv[2] : ReplaceExtension(input_path, ".gmap");

	std::vector<map_brush_t> brushes;
	std::vector<map_entity_t> entities;

	if (!ParseMapFile(input_path, brushes, entities))
	{
		std::cerr << "Failed To Parse '" << input_path << "'\n";
		return 1;
	}

	if (!WriteCompiledMap(output_path, brushes, entities))
	{
		std::cerr << "Failed To Write '" << output_path << "'\n";
		return 1;
	}

	std::cout << "Compiled '" << input_path << "' -> '" << output_path << "' (" << brushes.size() << " brushes, " << entities.size() << " entities)\n";

	return 0;
}