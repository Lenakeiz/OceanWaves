#pragma once

#define SEG_WIDTH 10.0f
#define MULTIPLIER 5

namespace octet
{

	const float GRAVITY = 9.81f;
	const double PI = 3.14159265358979323846264338327950288;

	class OceanMesh
	{

	private:

		struct VertexOcean
		{
			vec3p pos;
			vec3p norm;
			//vec3p originalposition;
			//vec2p textcoords;
			//vec3p origin;
		};

		VertexOcean* vertices;
		unsigned int vertCount;
		unsigned int* indices;
		unsigned int indexCount;

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
			unsigned int map_widht = _m * MULTIPLIER;
			unsigned int map_height = _n * MULTIPLIER;

			float terrain_width = SEG_WIDTH * (map_widht - 1);
			float terrain_eight = SEG_WIDTH * (map_height - 1);

			float half_terrain_width = terrain_width * 0.5f;
			float half_terrain_height = terrain_eight * 0.5f;

			oceanMesh = new mesh();

			vertices = new VertexOcean[map_widht * map_height];

			for (unsigned int j = 0; j < map_height; ++j)
			{
				for (unsigned int i = 0; i < map_widht; ++i)
				{
					unsigned int idx = i + (j * map_widht);
					
					float u = i / static_cast<float>(map_widht - 1);
					float v = j / static_cast<float>(map_height - 1);

					float x = (u * terrain_width) - half_terrain_width;
					float y = 0.0f;
					float z = (v * terrain_eight) - half_terrain_height;

					vertices[idx].norm = vec3p(0.0f, 1.0f, 0.0f);
					vertices[idx].pos = vec3p(x, y, z);
					++vertCount;
				}
			}

			// Generate indices.
			const unsigned int num_triangles = (map_widht - 1) * (map_height - 1) * 2;
			indices = new unsigned int[num_triangles * 3];

			unsigned int index = 0;
			for (unsigned int j = 0; j < map_height - 1; ++j)
			{
				for (unsigned int i = 0; i < map_widht - 1; ++i)
				{
					unsigned int vertex_index = i + (j * map_widht);
					indices[index++] = vertex_index;
					indices[index++] = vertex_index + map_widht;
					indices[index++] = vertex_index + 1;

					indices[index++] = vertex_index + map_widht;
					indices[index++] = vertex_index + map_widht + 1;
					indices[index++] = vertex_index + 1;
				}
			}
			indexCount = index - 1;
		
			SetupRendering();

		}

		~OceanMesh()
		{
		}

		void Init(){}

		void Update(){}

		void Draw(){}

	private:
		void SetupRendering()
		{
			oceanMesh->allocate(sizeof(VertexOcean) * vertCount, sizeof(uint32_t) * indexCount);
			oceanMesh->set_params(sizeof(VertexOcean), indexCount, vertCount, GL_TRIANGLES, GL_UNSIGNED_INT);

			// describe the structure of my_vertex to OpenGL
			oceanMesh->add_attribute(attribute_pos, 3, GL_FLOAT, 0);
			oceanMesh->add_attribute(attribute_normal, 3, GL_FLOAT, 12);
			oceanMesh->add_attribute(attribute_uv, 2, GL_FLOAT, 24);
			oceanMesh->set_params(32, 0, 0, GL_TRIANGLES, GL_UNSIGNED_INT);
		
			
		}
	};
}


