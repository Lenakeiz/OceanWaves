#pragma once

namespace octet
{

	class OceanMesh 
	{

	public:

		struct GerstnerWave
		{
		public:
			float wavelength;
			float amplitude;
			float speed;
			vec3 direction;
			float frequency;
			float phase;
			float steepness;

			GerstnerWave() = default;
			GerstnerWave(float w, float a, float s, vec3 d, float totSteep) : wavelength(w), amplitude(a), speed(s), direction(d)
			{
				frequency = 2 * (float)PI / wavelength;
				phase = speed * frequency;
				steepness = totSteep / (NUM_WAVES * frequency * amplitude);
			}

		public:			
			float GetHeightPosition(float x, float z, float t)
			{
				float displacement = direction.dot(vec3((float)x, 0.0f, (float)z));
				return amplitude * sinf(displacement * frequency + phase * t);
			}

			vec3 GetPosition(float x, float z, float t)
			{
				vec3 pos(0.0f,0.0f,0.0f);
				float displacement = direction.dot(vec3((float)x, 0.0f, (float)z));
				pos.y() = amplitude * sinf(displacement * frequency + phase * t);
				pos.x() = steepness * amplitude * direction.x() * cosf(displacement * frequency + phase * t);
				pos.z() = steepness * amplitude * direction.z() * cosf(displacement * frequency + phase * t);
				return pos;
			}

			vec3 GetNormal(float x, float z, float t, vec3 point)
			{
				vec3 nor(0.0f, 0.0f, 0.0f);
				float displacement = direction.dot(point);
				nor.y() -= steepness * amplitude * frequency * sinf(frequency * displacement + phase * t);
				nor.x() -= direction.x() * amplitude * frequency * cosf(frequency * displacement + phase * t);
				nor.z() -= direction.z() * amplitude * frequency * cosf(frequency * displacement + phase * t);
				return nor;
			}

		};
	private:
		//this struct is used to instruct the OpenGl and the vertex sader
		struct VertexOcean
		{
			vec3p pos;
			vec3p normal;
			uint32_t color;
		};

		dynarray<GerstnerWave> waves;
		GerstnerWave* prova;
		ref<mesh> oceanMesh;
		//other parameters
		float _totalSteepness;
		//Grid dimension

		uint32_t _m = 80;
		uint32_t _n = 80;
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
		random rand;

	public:

		OceanMesh()
		{
			oceanMesh = new mesh();
			prova = new GerstnerWave(50.0f, 2.0f, 1.0f, vec3(1.0, 0.0f, 1.0f).normalize(), 0.0f);
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

			_totalSteepness = rand.get(0.0f, 1.0f);
			GenerateGerstnerWaves(_totalSteepness);

			size_t num_vertices = map_widht * map_height;
			size_t num_indices = (map_widht - 1) * (map_height - 1) * 6;

			oceanMesh->allocate(sizeof(VertexOcean) * num_vertices, sizeof(uint32_t) * num_indices);
			oceanMesh->set_params(sizeof(VertexOcean), num_indices, num_vertices, GL_TRIANGLES, GL_UNSIGNED_INT);

			// describe the structure of my_vertex to OpenGL
			oceanMesh->add_attribute(attribute_pos, 3, GL_FLOAT, 0);
			oceanMesh->add_attribute(attribute_normal, 3, GL_FLOAT, 12);
			oceanMesh->add_attribute(attribute_color, 4, GL_UNSIGNED_BYTE, 24, GL_TRUE);

			param_shader *s = new param_shader("shaders/default.vs", "shaders/base_shader.fs");
			material *wm = new material(vec4(0.0f, 0.0f, 1.0f, 1), s);
			material *color = new material(vec4(0, 0, 1, 0.5f));
			scene_node *node = new scene_node();
			node->translate(vec3(0, -10.0f, 0));
			node->rotate(90.0f, vec3(0.0, 1.0f, 0.0f));

			_app->add_mesh_instance(new mesh_instance(node, oceanMesh, wm));
								
		}

		void FixedUpdate()
		{
			SetupRendering();
			SimulateOcean(fixedTime);
			//_simulationTime += deltaTime;
			++fixedTime;
		}

		void GenerateNewWaveSet()
		{
			_totalSteepness = rand.get(0.0f, 1.0f);
			for (size_t i = 0; i < NUM_WAVES; i++)
			{				
				waves[i].wavelength = rand.get(0.0f, 100.0f);
				waves[i].amplitude = rand.get(0.1f, 2.0f);
				waves[i].speed = rand.get(0.2f, 1.5f);
				waves[i].direction = vec3(rand.get(0.0f, 1.0f), 0.0f, rand.get(0.0f, 1.0f)).normalize();
				waves[i].frequency = 2 * PI / waves[i].wavelength;
				waves[i].phase = waves[i].speed * waves[i].frequency;
				waves[i].steepness = _totalSteepness / (NUM_WAVES * waves[i].frequency * waves[i].amplitude);
			}
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

		void GenerateGerstnerWaves(float steepNess)
		{
			for (size_t i = 0; i < NUM_WAVES; ++i)
			{
				GerstnerWave* gw = new GerstnerWave(rand.get(0.0f, 100.0f), rand.get(0.1f, 2.0f), rand.get(0.2f, 1.5f), vec3(rand.get(0.0f, 1.0f), 0.0f, rand.get(0.0f, 1.0f)).normalize(), steepNess);
				waves.push_back(*gw);
			}
		}

		vec3 GetGerstnerContribution(float x, float z, float t)
		{
			vec3 contribute(0.0f,0.0f,0.0f);
			for each (GerstnerWave gw in waves)
			{
				contribute += gw.GetPosition(x,z,t);
			}
			return contribute;
		}

		vec3 GetGerstnerNormal(float x, float z, float t, vec3 finalPoint)
		{
			vec3 contribute(0.0f, 1.0f, 0.0f);
			for each (GerstnerWave gw in waves)
			{
				contribute += gw.GetNormal(x, z, t, finalPoint);
			}
			return contribute;
		}

		void SetupRendering()
		{
			// describe the structure of my_vertex to OpenGL
			glClearColor(0, 0, 0, 1);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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

			for (unsigned int j = 0; j != map_height; ++j)
			{
				for (unsigned int i = 0; i != map_widht; ++i)
				{
					//uint32_t idx = i + (j * map_widht);

					float u = i / static_cast<float>(map_widht);
					float v = j / static_cast<float>(map_height);

					float x = (u * terrain_width) - half_terrain_width;
					float z = (v * terrain_eight) - half_terrain_height;
					
					//float y = prova->GetHeightPosition(x, z, t);

					/*VertexOcean vo;
					vo.norm = vec3p(0.0f, 1.0f, 0.0f);
					vo.pos = vec3p(x, y, z);

					vertices[idx] = vo;*/

					vec3 finalPosition = vec3(x, 0.0f, z) + GetGerstnerContribution(x, z, t);
					vtx->pos = vec3p(finalPosition);//vec3p(x, y, z);//
					//vec3 normalPosition = gerstner_wave_normals(wavePosition);
					vtx->normal = GetGerstnerNormal(x, z, t, finalPosition);
					vtx->color = make_color(vec3(0.0f,0.5f,1.0f));
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


