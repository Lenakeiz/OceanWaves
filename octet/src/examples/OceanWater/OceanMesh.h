#pragma once

#define SEG_WIDTH 1.0f
#define MULTIPLIER 1
#define PI 3.14159265358979323846264338327950288

#define NUM_WAVES 8

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
			float GetHeightPosition(float x, float z, float t)
			{
				float frequency = 2 * PI / wavelength;
				float phase = speed * frequency;
				float displacement = direction.dot(vec3((float)x, 0.0f, (float)z));
				return amplitude * sinf(displacement * frequency + phase * t);
			}

		};

		//this struct is used to instruct the OpenGl and the vertex sader
		struct VertexOcean
		{
			vec3p pos;
			vec3p norm;
			uint32_t color;
			//vec3p originalposition;
			//vec2p textcoords;
			//vec3p origin;
		};

		//dynarray<VertexOcean> vertices;
		//dynarray<uint32_t> indices;

		dynarray<GerstnerWave> waves;
		GerstnerWave* prova;
		ref<mesh> oceanMesh;
		//Grid dimension
		uint32_t _m = 128;
		uint32_t _n = 128;
		uint32_t map_widht;
		uint32_t map_height;
		float terrain_width;
		float terrain_eight;
		float half_terrain_width;
		float half_terrain_height;

		int mode = 5;

		//Not used cause updating using delta time step cause bad behaviour!
		float _simulationTime;
		unsigned long long fixedTime = 0;

		ref<visual_scene> _app;

	public:

		OceanMesh()
		{
			oceanMesh = new mesh();
			prova = new GerstnerWave(50.0f, 2.0f, 1.0f, vec3(1.0, 0.0f, 1.0f).normalize());
		}

		~OceanMesh()
		{
		}

		// this function converts three floats into a RGBA 8 bit color
		static uint32_t make_color(vec3 colour) {
			float r = colour.x();
			float g = colour.y();
			float b = colour.z();
			return 0xff000000 + ((int)(r*255.0f) << 0) + ((int)(g*255.0f) << 8) + ((int)(b*255.0f) << 16);
		}

		void Init(visual_scene *scene)
		{
			

			map_widht = _m * MULTIPLIER;
			map_height = _n * MULTIPLIER;

			terrain_width = SEG_WIDTH * (map_widht - 1);
			terrain_eight = SEG_WIDTH * (map_height - 1);

			half_terrain_width = terrain_width * 0.5f;
			half_terrain_height = terrain_eight * 0.5f;
			
			this->_app = scene;

			size_t num_vertices = map_widht * map_height;
			size_t num_indices = (map_widht - 1) * (map_height - 1) * 6;

			oceanMesh->allocate(sizeof(VertexOcean) * num_vertices, sizeof(uint32_t) * num_indices);
			oceanMesh->set_params(sizeof(VertexOcean), num_indices, num_vertices, GL_TRIANGLES, GL_UNSIGNED_INT);
			//vertices.reserve(num_vertices);// = new VertexOcean[map_widht * map_height];
			//indices.reserve(num_indices);

			// describe the structure of my_vertex to OpenGL
			oceanMesh->add_attribute(attribute_pos, 3, GL_FLOAT, 0);
			oceanMesh->add_attribute(attribute_normal, 3, GL_FLOAT, 12);
			oceanMesh->add_attribute(attribute_color, 4, GL_UNSIGNED_BYTE, 24, GL_TRUE);

			param_shader *s = new param_shader("shaders/default.vs", "shaders/simple_color.fs");
			material *wm = new material(vec4(1.0f, 0.0f, 0.0f, 1), s);
			material *color = new material(vec4(0, 0, 1, 0.5f));
			scene_node *node = new scene_node();
			node->translate(vec3(0, -10.0f, 0));
			//node->rotate(90.0f, vec3(1.0, 0.0f, 0.0f));

			_app->add_mesh_instance(new mesh_instance(node, oceanMesh, color));
								
		}

		void FixedUpdate()
		{
			SetupRendering();
			SimulateOcean(fixedTime);
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
			// describe the structure of my_vertex to OpenGL
			oceanMesh->clear_attributes();

			oceanMesh->add_attribute(attribute_pos, 3, GL_FLOAT, 0);
			oceanMesh->add_attribute(attribute_normal, 3, GL_FLOAT, 12);
			oceanMesh->add_attribute(attribute_color, 4, GL_UNSIGNED_BYTE, 24, GL_TRUE);

			//oceanMesh->add_attribute(attribute_uv, 2, GL_FLOAT, 24);
			//oceanMesh->set_params(32, 0, 0, GL_TRIANGLES, GL_UNSIGNED_INT);
			oceanMesh->set_mode(mode);
		}

		void SimulateOcean(float t)
		{

			gl_resource::wolock vl(oceanMesh->get_vertices());
			VertexOcean* vtx = (VertexOcean *)vl.u8();
			gl_resource::wolock il(oceanMesh->get_indices());
			uint32_t *idx = il.u32();

			//// make the vertices
			//for (size_t i = 0; i != mesh_size; ++i) {
			//	for (size_t j = 0; j != mesh_size; ++j) {
			//		vec3 wavePosition = gerstner_wave_position(j, i);
			//		vtx->pos = vec3p(vec3(1.0f * j, -1.0f * i, 0.0f) + wavePosition);
			//		vec3 normalPosition = gerstner_wave_normals(wavePosition);
			//		vtx->normal = vec3p(wavePosition);
			//		vtx->color = make_color(sine_waves[0].colour);
			//		vtx++;
			//	}
			//}

			//// make the triangles
			//uint32_t vn = 0;
			//for (size_t i = 0; i != mesh_size * (mesh_size - 1); ++i) {
			//	if (i % mesh_size != mesh_size - 1){
			//		idx[0] = i;
			//		idx[1] = i + mesh_size + 1;
			//		idx[2] = i + 1;
			//		idx += 3;

			//		idx[0] = i;
			//		idx[1] = i + mesh_size;
			//		idx[2] = i + mesh_size + 1;
			//		idx += 3;
			//	}
			//}

			for (unsigned int j = 0; j < map_height; ++j)
			{
				for (unsigned int i = 0; i < map_widht; ++i)
				{
					//uint32_t idx = i + (j * map_widht);

					float u = i / static_cast<float>(map_widht - 1);
					float v = j / static_cast<float>(map_height - 1);

					float x = (u * terrain_width) - half_terrain_width;
					float z = (v * terrain_eight) - half_terrain_height;
					//float y = 0.0f;
					float y = prova->GetHeightPosition(x, z, t);

					/*VertexOcean vo;
					vo.norm = vec3p(0.0f, 1.0f, 0.0f);
					vo.pos = vec3p(x, y, z);

					vertices[idx] = vo;*/

					vec3 wavePosition = vec3(0.0f, prova->GetHeightPosition(x, z, t), 0.0f);//gerstner_wave_position(j, i);
					vtx->pos = vec3p(vec3(1.0f * j, 0.0f, -1.0f * i) + wavePosition);//vec3p(x, y, z);//
					//vec3 normalPosition = gerstner_wave_normals(wavePosition);
					vtx->norm = vec3p(0.0f, 1.0f, 0.0f);//vec3p(wavePosition);
					vtx->color = make_color(vec3(0.0f,0.3f,1.0f));
					vtx++;
				}

			}

			// make the triangles
			for (size_t i = 0; i != map_height * (map_height - 1); ++i) {
				if (i % map_widht != map_widht - 1){
					idx[0] = i;
					idx[1] = i + map_widht + 1;
					idx[2] = i + 1;
					idx += 3;
					idx[0] = i;
					idx[1] = i + map_widht;
					idx[2] = i + map_widht + 1;
					idx += 3;
				}
			}
		}

	};
}


