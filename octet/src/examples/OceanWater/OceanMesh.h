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
			GerstnerWave(float w, float a, float s, vec3 d) :wavelength(w), amplitude(a), speed(s), direction(d){}

		public:
			float GetHeightPosition(float x, float z, float time)
			{
				float frequency = 2 * PI / wavelength;
				float omega = speed * frequency;
				float displacement = direction.dot(vec3((float)x, 0.0f, (float)z));
				return amplitude * sinf(displacement * frequency + omega * time);
			}

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
		dynarray<vec3p> points;
		uint32_t vertCount;
		dynarray<uint32_t> indices;
		uint32_t indexCount;

		GerstnerWave* gw;
		ref<mesh> oceanMesh;
		int mode = 5;
		float _simulationTime;
		unsigned long long fixedTime = 0;		//Grid dimension
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
			gw = new GerstnerWave(20.0f, 8.0f, 0.5f, vec3(1.0f, 0.0f, 1.0f).normalize());
		}

		~OceanMesh()
		{
			delete gw;
			gw = NULL;
		}

		void Init()
		{
			unsigned int map_widht = _m * MULTIPLIER;
			unsigned int map_height = _n * MULTIPLIER;

			float terrain_width = SEG_WIDTH * (map_widht - 1);
			float terrain_eight = SEG_WIDTH * (map_height - 1);

			float half_terrain_width = terrain_width * 0.5f;
			float half_terrain_height = terrain_eight * 0.5f;
			
			size_t num_vertices = map_widht * map_height;
			size_t num_indices = (map_widht - 1) * (map_height - 1) * 6;
			oceanMesh->allocate(sizeof(VertexOcean) * num_vertices, sizeof(uint32_t) * num_indices);
			oceanMesh->set_params(sizeof(VertexOcean), num_indices, num_vertices, GL_TRIANGLES, GL_UNSIGNED_INT);
			//vertices.resize(map_widht * map_height);// = new VertexOcean[map_widht * map_height];
			
			points.resize(map_widht * map_height);

			oceanMesh->add_attribute(attribute_pos, 3, GL_FLOAT, 0);
			oceanMesh->add_attribute(attribute_normal, 3, GL_FLOAT, 12);

			for (unsigned int j = 0; j != map_height; ++j)
			{
				for (unsigned int i = 0; i != map_widht; ++i)
				{
					unsigned int idx = i + (j * map_widht);

					float u = i / static_cast<float>(map_widht - 1);
					float v = j / static_cast<float>(map_height - 1);

					float x = (u * terrain_width) - half_terrain_width;
					float y = 0.0f;
					float z = (v * terrain_eight) - half_terrain_height;

					/*VertexOcean vo;
					vo.norm = vec3p(0.0f, 1.0f, 0.0f);
					vo.pos = vec3p(x, y, z);

					vertices[idx] = vo;*/

					points[idx] = vec3p(x, y, z);

					//vertices[idx].norm = vec3p(0.0f, 1.0f, 0.0f);
					//vertices[idx].pos = vec3p(x, y, z);
					++vertCount;
				}

			}

			

			// Generate indices.
			//const unsigned int num_triangles = (map_widht - 1) * (map_height - 1) * 2;
			//indices.resize(num_triangles * 3);//  = new unsigned int[num_triangles * 3];

			//unsigned int index = 0;
			//for (unsigned int j = 0; j < map_height - 1; ++j)
			//{
			//	for (unsigned int i = 0; i < map_widht - 1; ++i)
			//	{
			//		unsigned int vertex_index = i + (j * map_widht);

			//		indices[index++] = vertex_index;
			//		indices[index++] = (vertex_index + map_widht);
			//		indices[index++] = (vertex_index + 1);

			//		indices[index++] = (vertex_index + map_widht);
			//		indices[index++] = (vertex_index + map_widht + 1);
			//		indices[index++] = (vertex_index + 1);
			//	}
			//}

			//indexCount = index - 1;

			SetupRendering();
		}

		void Update(float deltaTime)
		{
			SimulateOcean(fixedTime, 1.0f / (MULTIPLIER * SEG_WIDTH));
			SetupRendering();
			//_simulationTime += deltaTime;
			++fixedTime;
		}

		//GET/SET FUNTIONS
		mesh* getMeshInstance()
		{
			return oceanMesh;
		}

		void SetMode(int m)
		{
			mode = m;
		}

	private:
		void SetupRendering()
		{
			unsigned int map_widht = _m * MULTIPLIER;
			unsigned int map_height = _n * MULTIPLIER;
			//// describe the structure of my_vertex to OpenGL
			//oceanMesh->clear_attributes();

			//oceanMesh->add_attribute(attribute_uv, 2, GL_FLOAT, 24);
			//oceanMesh->set_params(32, 0, 0, GL_TRIANGLES, GL_UNSIGNED_INT);
		
			gl_resource::wolock vl(oceanMesh->get_vertices());
			VertexOcean *vtx = (VertexOcean *)vl.u8();
			gl_resource::wolock il(oceanMesh->get_indices());
			uint32_t *idx = il.u32();

			for (unsigned int j = 0; j != map_height; ++j)
			{
				for (unsigned int i = 0; i != map_widht; ++i)
				{
					unsigned int idx = i + (j * map_widht);
					vtx->pos = points[idx];
					vtx->norm = vec3p(0.0f, 1.0f, 0.0f);
					vtx++;
				}
			}

			//uint32_t vn = 0;

			for (size_t i = 0; i != map_height * (map_height - 1) ; ++i) {
				// 0--2
				// | \|
				// 1--3
				if (i % map_widht != map_widht - 1){
					idx[0] = i;
					idx[1] = i + map_widht + 1;
					idx[2] = i + 1;
					idx[3] = i;
					idx[4] = i + map_widht;
					idx[5] = i + map_widht + 1;
					idx += 6;
				}
			}
			//oceanMesh->set_vertices(vertices);
			//oceanMesh->set_indices(indices);
			
			oceanMesh->set_mode(mode);
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
					float z = (v * terrain_eight) - half_terrain_height;
					float y = 0.0f;
					//float y = gw->GetHeightPosition(x, z, t);

					/*VertexOcean vo;
					vo.norm = vec3p(0.0f, 1.0f, 0.0f);
					vo.pos = vec3p(x, y, z);

					vertices[idx] = vo;*/

					points[i + (j * map_widht)] = vec3p(x, y, z);

					//vertices[idx].norm = vec3p(0.0f, 1.0f, 0.0f);
					//vertices[idx].pos = vec3p(x, y, z);
					++vertCount;
				}

			}
		}

	};
}


