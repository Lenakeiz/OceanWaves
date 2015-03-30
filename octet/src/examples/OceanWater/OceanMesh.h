#pragma once

#define SEG_WIDTH 1.0f
#define MULTIPLIER 1
#define PI 3.14159265358979323846264338327950288

namespace octet
{

	class OceanMesh
	{

	private:

		struct GerstnerWave
		{
			float wavelength;
			float amplitude;
			float speed;
			vec3 direction;

			GerstnerWave() = default;
			GerstnerWave(float w, float a, float s, float d) :wavelength(w), amplitude(a), speed(s), direction(d){}

		};

		struct VertexOcean
		{
			vec3p pos;
			vec3p norm;
			//vec3p originalposition;
			//vec2p textcoords;
			//vec3p origin;
		};

		dynarray<VertexOcean> vertices;
		uint32_t vertCount;
		dynarray<uint32_t> indices;
		uint32_t indexCount;

		ref<mesh> oceanMesh;

		float _simulationTime;

		//Grid dimension: Tessendorf approach
		unsigned int _m = 128;
		unsigned int _n = 128;
		//Spatial dimensions of grid
		float _xdim;
		float _ydim;

		//Tessendorf Simulation base parameters
		float _speed;
		float _L_Length;
		float _l_Length;
		float _amplitude;

	public:

		OceanMesh() : _speed(4.0f), _L_Length(2.0f), _amplitude(1.0f)
		{
			oceanMesh = new mesh();
		}

		~OceanMesh()
		{
		}

		void Init()
		{
			unsigned int map_widht = _m * MULTIPLIER;
			unsigned int map_height = _n * MULTIPLIER;

			float terrain_width = SEG_WIDTH * (map_widht - 1);
			float terrain_eight = SEG_WIDTH * (map_height - 1);

			float half_terrain_width = terrain_width * 0.5f;
			float half_terrain_height = terrain_eight * 0.5f;
			
			//vertices = new VertexOcean[map_widht * map_height];

			for (unsigned int j = 0; j < map_height; ++j)
			{
				for (unsigned int i = 0; i < map_widht; ++i)
				{
					//unsigned int idx = i + (j * map_widht);

					float u = i / static_cast<float>(map_widht - 1);
					float v = j / static_cast<float>(map_height - 1);

					float x = (u * terrain_width) - half_terrain_width;
					float y = 0.0f;
					float z = (v * terrain_eight) - half_terrain_height;

					VertexOcean vo;
					vo.norm = vec3p(0.0f, 1.0f, 0.0f);
					vo.pos = vec3p(x, y, z);

					vertices.push_back(vo);

					//vertices[idx].norm = vec3p(0.0f, 1.0f, 0.0f);
					//vertices[idx].pos = vec3p(x, y, z);
					++vertCount;
				}

			}

			// Generate indices.
			const unsigned int num_triangles = (map_widht - 1) * (map_height - 1) * 2;
			//indices = new unsigned int[num_triangles * 3];

			//unsigned int index = 0;
			for (unsigned int j = 0; j < map_height - 1; ++j)
			{
				for (unsigned int i = 0; i < map_widht - 1; ++i)
				{
					unsigned int vertex_index = i + (j * map_widht);

					indices.push_back(vertex_index);
					indices.push_back(vertex_index + map_widht);
					indices.push_back(vertex_index + 1);

					indices.push_back(vertex_index + map_widht);
					indices.push_back(vertex_index + map_widht + 1);
					indices.push_back(vertex_index + 1);
				}
			}

			//indexCount = index - 1;

			SetupRendering();
		}

		void Update(float deltaTime)
		{
			SimulateOcean(_simulationTime, 1.0f / (MULTIPLIER * SEG_WIDTH));
			//SetupRendering();
			_simulationTime += deltaTime;
		}

		void Draw(){}

		mesh* getMeshInstance()
		{
			return oceanMesh;
		}

	private:
		void SetupRendering()
		{
			//oceanMesh->allocate(sizeof(VertexOcean) * vertCount, sizeof(uint32_t) * indexCount);
			//oceanMesh->set_params(sizeof(VertexOcean), indexCount, vertCount, GL_TRIANGLES, GL_UNSIGNED_INT);

			//oceanMesh->allocate(sizeof(vert)*plane_.size(), sizeof(unsigned)*indicies.size());
			//oceanMesh->set_params(sizeof(vert), indicies.size(), plane_.size(), GL_TRIANGLES, GL_UNSIGNED_INT);
			//oceanMesh->clear_attributes();
			//oceanMesh->add_attribute(octet::attribute_pos, 3, GL_FLOAT, 0);
			//oceanMesh->add_attribute(octet::attribute_normal, 3, GL_FLOAT, 12, 0);
			//oceanMesh->add_attribute(octet::attribute_color, 4, GL_UNSIGNED_BYTE, 24, TRUE);

			//octet::gl_resource::wolock vl(mesh_->get_vertices());
			//octet::gl_resource::wolock il(mesh_->get_indices());
			//unsigned * indxP = (unsigned*)il.u8();
			//vert *vtxP = (vert*)vl.u8();

			//// describe the structure of my_vertex to OpenGL
			oceanMesh->clear_attributes();
			oceanMesh->add_attribute(attribute_pos, 3, GL_FLOAT, 0);
			oceanMesh->add_attribute(attribute_normal, 3, GL_FLOAT, 12);
			oceanMesh->add_attribute(attribute_uv, 2, GL_FLOAT, 24);
			oceanMesh->set_params(32, 0, 0, GL_TRIANGLES, GL_UNSIGNED_INT);
		
			oceanMesh->set_vertices(vertices);
			oceanMesh->set_indices(indices);
			
		}

		void SimulateOcean(float t, float scale)
		{
			vertCount = 0;

			unsigned int map_widht = _m * MULTIPLIER;
			unsigned int map_height = _n * MULTIPLIER;

			float terrain_width = SEG_WIDTH * (map_widht - 1);
			float terrain_eight = SEG_WIDTH * (map_height - 1);

			float half_terrain_width = terrain_width * 0.5f;
			float half_terrain_height = terrain_eight * 0.5f;


			for (unsigned int j = 0; j < map_height; ++j)
			{
				for (unsigned int i = 0; i < map_widht; ++i)
				{
					//unsigned int idx = i + (j * map_widht);

					float u = i / static_cast<float>(map_widht - 1);
					float v = j / static_cast<float>(map_height - 1);

					float x = (u * terrain_width) - half_terrain_width;
					float y = 0.0f;
					float z = (v * terrain_eight) - half_terrain_height;

					VertexOcean vo;
					vo.norm = vec3p(0.0f, 1.0f, 0.0f);
					vo.pos = vec3p(x, y, z);

					vertices.push_back(vo);

					//vertices[idx].norm = vec3p(0.0f, 1.0f, 0.0f);
					//vertices[idx].pos = vec3p(x, y, z);
					++vertCount;
				}

			}
		}

	};
}


