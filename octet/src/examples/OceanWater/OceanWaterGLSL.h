#include "OceanTerrainMesh.h"

namespace octet
{
	class OceanWaterGLSL : public app
	{
		// scene for drawing box
		ref<visual_scene> app_scene;

		mouse_look mouse_look_helper;
		ref<camera_instance> camera;

		float update_time = 0;
		
		//mesh terrain
		ref<mesh> terrainmesh;
		OceanTerrainMesh::water_geometry_source water_source;
		int n_waves;
		dynarray<OceanTerrainMesh::GerstnerWave> waves;
		ref<material> water_material;
		random rand;

		float _totalSteepness = 0.0f;

		//mapped for uniform
		float wavelength[8];
		float amplitude[8];
		float speed[8];
		float dir_x[8];
		float dir_z[8];
		float steepness[8];
		
		//uniforms sections: vertex shader
		ref<param_uniform> uniform_time;
		ref<param_uniform> uniform_n_waves;

		//uniforms sections: uniform
		ref<param_uniform> uniform_wavelength;
		ref<param_uniform> uniform_amplitude;
		ref<param_uniform> uniform_speed;
		ref<param_uniform> uniform_dir_x;
		ref<param_uniform> uniform_dir_z;
		ref<param_uniform> uniform_steepness;

		void initialize_waves()
		{
			_totalSteepness = rand.get(0.0f, 1.0f);
			n_waves = NUM_WAVES;
			for (size_t i = 0; i < n_waves; ++i)
			{
				OceanTerrainMesh::GerstnerWave* gw = new OceanTerrainMesh::GerstnerWave(rand.get(0.0f, 100.0f), rand.get(0.1f, 2.0f), rand.get(0.2f, 1.5f), vec3(rand.get(0.0f, 1.0f), 0.0f, rand.get(0.0f, 1.0f)).normalize(), _totalSteepness);
				waves.push_back(*gw);
			}
		}
		
		void update_uniform_variables()
		{
			for (size_t i = 0; i < 8; ++i)
			{
				if (i < n_waves)
				{
					wavelength[i]	= waves[i].wavelength;
					amplitude[i]	= waves[i].amplitude;
					speed[i]		= waves[i].speed;
					dir_x[i]		= waves[i].direction.x();
					dir_z[i]		= waves[i].direction.z();
					steepness[i]	= waves[i].steepness;
				}
				else
				{	
					wavelength[i] = 0.0f;
					amplitude[i] = 0.0f;
					speed[i] = 0.0f;
					dir_x[i] = 0.0f;
					dir_z[i] = 0.0f;
					steepness[i] = 0.0f;
				}				
			}
		}
		
		void register_uniforms()
		{
			//Fixed Time
			atom_t time_atom = app_utils::get_atom("time");
			uniform_time = water_material->add_uniform(&update_time, time_atom, GL_FLOAT, 1, param::stage_vertex);
			//Number of Waves
			atom_t atom_n_waves = app_utils::get_atom("n_waves");
			uniform_n_waves = water_material->add_uniform(nullptr, atom_n_waves, GL_INT, 1, param::stage_vertex);
			water_material->set_uniform(uniform_n_waves, &n_waves, sizeof(int));

			//Registering waves parameters
			atom_t a_wavelength = app_utils::get_atom("wavelength");
			uniform_wavelength = water_material->add_uniform(nullptr, a_wavelength, GL_FLOAT, 8, param::stage_vertex);
			atom_t a_amplitude = app_utils::get_atom("amplitude");
			uniform_amplitude = water_material->add_uniform(nullptr, a_amplitude, GL_FLOAT, 8, param::stage_vertex);
			atom_t a_speed = app_utils::get_atom("speed");
			uniform_speed = water_material->add_uniform(nullptr, a_speed, GL_FLOAT, 8, param::stage_vertex);
			atom_t a_dir_x = app_utils::get_atom("dir_x");
			uniform_dir_x = water_material->add_uniform(nullptr, a_dir_x, GL_FLOAT, 8, param::stage_vertex);
			atom_t a_dir_z = app_utils::get_atom("dir_z");
			uniform_dir_z = water_material->add_uniform(nullptr, a_dir_z, GL_FLOAT, 8, param::stage_vertex);
			atom_t a_steepness = app_utils::get_atom("steepness");
			uniform_steepness = water_material->add_uniform(nullptr, a_steepness, GL_FLOAT, 8, param::stage_vertex);
		}
		
		void update_uniforms()
		{
			water_material->set_uniform(uniform_wavelength, wavelength, 8 * sizeof(float));
			water_material->set_uniform(uniform_amplitude, amplitude, 8 * sizeof(float));
			water_material->set_uniform(uniform_speed, speed, 8 * sizeof(float));
			water_material->set_uniform(uniform_dir_x, dir_x, 8 * sizeof(float));
			water_material->set_uniform(uniform_dir_z, dir_z, 8 * sizeof(float));
			water_material->set_uniform(uniform_steepness, steepness, 8 * sizeof(float));
		}

		void GenerateNewWaveSet()
		{
			_totalSteepness = rand.get(0.0f, 1.0f);
			for (size_t i = 0; i < n_waves; i++)
			{
				waves[i].wavelength = rand.get(0.0f, 100.0f);
				waves[i].amplitude = rand.get(0.1f, 2.0f);
				waves[i].speed = rand.get(0.2f, 1.5f);
				waves[i].direction = vec3(rand.get(0.0f, 1.0f), 0.0f, rand.get(0.0f, 1.0f)).normalize();
				//waves[i].frequency = 2 * PI / waves[i].wavelength;
				//waves[i].phase = waves[i].speed * waves[i].frequency;
				waves[i].steepness = _totalSteepness / (n_waves * waves[i].frequency * waves[i].amplitude);
			}
			update_uniform_variables();
			update_uniforms();
		}

		void keyboard(){
			if (is_key_down(key::key_esc))
			{
				exit(1);
			}
			else if (is_key_going_down('1')){
				terrainmesh->set_mode(1);
			}
			else if (is_key_going_down('2')){
				terrainmesh->set_mode(4);
			}
			else if (is_key_going_down('3')){
				terrainmesh->set_mode(0);
			}
			else if (is_key_down('W')){
				camera->get_node()->translate(vec3(0, 0, -5));
			}
			else if (is_key_down('S')){
				camera->get_node()->translate(vec3(0, 0, 5));
			}
			else if (is_key_down('A')){
				camera->get_node()->translate(vec3(-5, 0, 0));
			}
			else if (is_key_down('D')){
				camera->get_node()->translate(vec3(5, 0, 0));
			}
			else if (is_key_down('E')){
				camera->get_node()->translate(vec3(0, 5, 0));
			}
			else if (is_key_down('Q')){
				camera->get_node()->translate(vec3(0, -5, 0));
			}
			//new generation
			else if (is_key_down('G')){
				GenerateNewWaveSet();
			}
		}

		void mouse(){
			//if (is_key_down(key::key_shift))
			{
				scene_node *camera_node = camera->get_node();
				mat4t &camera_to_world = camera_node->access_nodeToParent();
				mouse_look_helper.update(camera_to_world);
			}
		}

		void add_light_instances()
		{
			light *_light = new light();
			light_instance *li = new light_instance();
			scene_node *node = new scene_node();
			app_scene->add_child(node);
			node->translate(vec3(100, 100, 100));
			node->rotate(45, vec3(1, 0, 0));
			node->rotate(45, vec3(0, 1, 0));
			_light->set_color(vec4(1, 1, 1, 1));
			_light->set_kind(atom_directional);
			li->set_node(node);
			li->set_light(_light);
			app_scene->add_light_instance(li);
			node = new scene_node();
			app_scene->add_child(node);
			_light = new light();
			li = new light_instance();
			node->translate(vec3(-100, 100, -100));
			node->rotate(45, vec3(1, 0, 0));
			node->rotate(-45, vec3(0, 1, 0));
			_light->set_color(vec4(1, 1, 1, 1));
			_light->set_kind(atom_directional);
			li->set_node(node);
			li->set_light(_light);
			app_scene->add_light_instance(li);
			//light *l = new light();
			//light_instance *li = new light_instance();
			//scene_node *n = new scene_node();
			//app_scene->add_child(n);
			//n->translate(vec3(0.0f, 50, -50));
			//n->rotate(-45, vec3(1, 0, 0));
			////n->rotate(-180, vec3(0, 1, 0));
			//l->set_color(vec4(1, 1, 1, 1));
			//l->set_kind(atom_directional);
			//li->set_node(n);
			//li->set_light(l);
			//app_scene->add_light_instance(li);
		}

	public:
		/// this is called when we construct the class before everything is initialised.
		OceanWaterGLSL(int argc, char **argv) : app(argc, argv) {
		}

		/// this is called once OpenGL is initialized
		void app_init() {

			mouse_look_helper.init(this, 200.0f / 360.0f, false);
			app_scene = new visual_scene();
			app_scene->create_default_camera_and_lights();
			camera = app_scene->get_camera_instance(0);
			camera->get_node()->translate(vec3(10, 10, 0));
			camera->set_far_plane(10000);

			add_light_instances();

			mat4t mat;
			mat.loadIdentity();
			//mat.translate(0, -0.5f, 0);

			initialize_waves();
			update_uniform_variables();		

			param_shader* water_shader = new param_shader("shaders/basewater.vs", "shaders/default_solid.fs");
			water_material = new material(vec4(0.0f, 0.0f, 1.0f, 1.0f), water_shader);
			
			register_uniforms();
			update_uniforms();

			terrainmesh = new mesh_terrain(vec3(1000.0f, 0.0f, 1000.0f), ivec3(400, 1, 400), water_source);

			app_scene->add_shape(
				mat,
				terrainmesh,
				water_material,
				false, 0
				);
		}

		/// this is called to draw the world
		void draw_world(int x, int y, int w, int h) {
			
			
			int vx = 0, vy = 0;
			get_viewport_size(vx, vy);
			app_scene->begin_render(vx, vy);

			keyboard();
			mouse();

			glClearColor(0, 0, 0, 1);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			water_material->set_uniform(uniform_time, &update_time, sizeof(update_time));

			// update matrices. assume 30 fps.
			app_scene->update(1.0f / 30);
			
			// draw the scene
			app_scene->render((float)vx / vy); 
			update_time += 1.0f;		}
	};

}

