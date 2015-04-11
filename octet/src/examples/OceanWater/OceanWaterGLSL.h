#include "OceanTerrainMesh.h"

namespace octet
{
	void TW_CALL GenerateRandom(void* clientData)
	{
		bool* makeRandom = (bool*)clientData;
		*makeRandom  = true;
	}


	class OceanWaterGLSL : public app
	{
		//anttweakbar
		TwBar* tweakBar;
		// scene for drawing box
		ref<visual_scene> app_scene;

		mouse_look mouse_look_helper;
		ref<camera_instance> camera;

		float update_time = 0;
		
		//skydome
		bool skybox = true;

		//mesh terrain
		ref<mesh> terrainmesh;
		float dimension = 1200.0f;
		int precision = 800;
		OceanTerrainMesh::water_geometry_source water_source;
		int n_waves;
		dynarray<OceanTerrainMesh::GerstnerWave> waves;
		ref<material> water_material;
		vec3 oceancolor;
		random rand;

		bool doRandom;

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
		
		void initializes_bar()
		{
			TwInit(TW_OPENGL, NULL);
			tweakBar = TwNewBar("Gerstner Wave Configurator");

			//TwDefine("Gerstner refresh=0.2");
			TwAddVarRO(tweakBar, "Time", TW_TYPE_FLOAT, &update_time, "help ='Simulation time for the waves'");
			TwAddVarRW(tweakBar, "Ocean Colour", TW_TYPE_COLOR3F, &oceancolor, " label='Color' ");
			TwAddVarRW(tweakBar, "NumWAves", TW_TYPE_INT32, &n_waves,"label ='Number of Waves' min=1 max=8 step=1 help='Set the number of total waves used for simulation'");
			/*TwAddVarRW(tweakBar, "Total Steepness", TW_TYPE_FLOAT, &_totalSteepness, "label ='Total steepness' min=0.01 max=1.0 step=0.01 help ='Total Steepness used for the sample");*/
			TwAddButton(tweakBar, "Random generation", GenerateRandom, &doRandom, "help='Generate Random Set for the waves' ");

#pragma region Gerstner waves data			

			TwAddVarRW(tweakBar, "Wavelength1", TW_TYPE_FLOAT, &wavelength[0], " label='Wavelength' group='Gerstner 1' min=0.1 max=500.0 step=0.1");
			TwAddVarRW(tweakBar, "Amplitude1", TW_TYPE_FLOAT, &amplitude[0], " label='Amplitude' group='Gerstner 1' min=0.01 max=7.0 step=0.1");
			TwAddVarRW(tweakBar, "Speed1", TW_TYPE_FLOAT, &speed[0]," label='Speed' group='Gerstner 1' min=0.01 max=5.0 step=0.1");
			TwAddVarRW(tweakBar, "DirectionXAxis1", TW_TYPE_FLOAT, &dir_x[0], " label='Direction x axis' group='Gerstner 1' min=-1.0 max=1.0 step=0.01");
			TwAddVarRW(tweakBar, "DirectionZAxis1", TW_TYPE_FLOAT, &dir_z[0], " label='Direction z axis' group='Gerstner 1' min=-1.0 max=1.0 step=0.01");
			TwAddVarRW(tweakBar, "Steepness1", TW_TYPE_FLOAT, &steepness[0], " label='Steepness' group='Gerstner 1' min=0.0 max=250.0 step=0.1");
			
			TwAddVarRW(tweakBar, "Wavelength2", TW_TYPE_FLOAT, &wavelength[1], " label='Wavelength' group='Gerstner 2' min=0.1 max=500.0 step=0.1");
			TwAddVarRW(tweakBar, "Amplitude2", TW_TYPE_FLOAT, &amplitude[1], " label='Amplitude' group='Gerstner 2' min=0.01 max=7.0 step=0.1");
			TwAddVarRW(tweakBar, "Speed2", TW_TYPE_FLOAT, &speed[1], " label='Speed' group='Gerstner 2' min=0.01 max=5.0 step=0.1");
			TwAddVarRW(tweakBar, "DirectionXAxis2", TW_TYPE_FLOAT, &dir_x[1], " label='Direction x axis' group='Gerstner 2' min=-1.0 max=1.0 step=0.01");
			TwAddVarRW(tweakBar, "DirectionZAxis2", TW_TYPE_FLOAT, &dir_z[1], " label='Direction z axis' group='Gerstner 2' min=-1.0 max=1.0 step=0.01");
			TwAddVarRW(tweakBar, "Steepness2", TW_TYPE_FLOAT, &steepness[1], " label='Steepness' group='Gerstner 2' min=0.0 max=250.0 step=0.1");
			
			TwAddVarRW(tweakBar, "Wavelength3", TW_TYPE_FLOAT, &wavelength[2], " label='Wavelength' group='Gerstner 3' min=0.1 max=500.0 step=0.1");
			TwAddVarRW(tweakBar, "Amplitude3", TW_TYPE_FLOAT, &amplitude[2], " label='Amplitude' group='Gerstner 3' min=0.01 max=7.0 step=0.1");
			TwAddVarRW(tweakBar, "Speed3", TW_TYPE_FLOAT, &speed[2], " label='Speed' group='Gerstner 3' min=0.01 max=5.0 step=0.1");
			TwAddVarRW(tweakBar, "DirectionXAxis3", TW_TYPE_FLOAT, &dir_x[2], " label='Direction x axis' group='Gerstner 3' min=-1.0 max=1.0 step=0.01");
			TwAddVarRW(tweakBar, "DirectionZAxis3", TW_TYPE_FLOAT, &dir_z[2], " label='Direction z axis' group='Gerstner 3' min=-1.0 max=1.0 step=0.01");
			TwAddVarRW(tweakBar, "Steepness3", TW_TYPE_FLOAT, &steepness[2], " label='Steepness' group='Gerstner 3' min=0.0 max=250.0 step=0.1");
			
			TwAddVarRW(tweakBar, "Wavelength4", TW_TYPE_FLOAT, &wavelength[3], " label='Wavelength' group='Gerstner 4' min=0.1 max=500.0 step=0.1");
			TwAddVarRW(tweakBar, "Amplitude4", TW_TYPE_FLOAT, &amplitude[3], " label='Amplitude' group='Gerstner 4' min=0.01 max=7.0 step=0.1");
			TwAddVarRW(tweakBar, "Speed4", TW_TYPE_FLOAT, &speed[3], " label='Speed' group='Gerstner 4' min=0.01 max=5.0 step=0.1");
			TwAddVarRW(tweakBar, "DirectionXAxis4", TW_TYPE_FLOAT, &dir_x[3], " label='Direction x axis' group='Gerstner 4' min=-1.0 max=1.0 step=0.01");
			TwAddVarRW(tweakBar, "DirectionZAxis4", TW_TYPE_FLOAT, &dir_z[3], " label='Direction z axis' group='Gerstner 4' min=-1.0 max=1.0 step=0.01");
			TwAddVarRW(tweakBar, "Steepness4", TW_TYPE_FLOAT, &steepness[3], " label='Steepness' group='Gerstner 4' min=0.0 max=250.0 step=0.1");
			
			TwAddVarRW(tweakBar, "Wavelength5", TW_TYPE_FLOAT, &wavelength[4], " label='Wavelength' group='Gerstner 5' min=0.1 max=500.0 step=0.1");
			TwAddVarRW(tweakBar, "Amplitude5", TW_TYPE_FLOAT, &amplitude[4], " label='Amplitude' group='Gerstner 5' min=0.01 max=7.0 step=0.1");
			TwAddVarRW(tweakBar, "Speed5", TW_TYPE_FLOAT, &speed[4], " label='Speed' group='Gerstner 5' min=0.01 max=5.0 step=0.1");
			TwAddVarRW(tweakBar, "DirectionXAxis5", TW_TYPE_FLOAT, &dir_x[4], " label='Direction x axis' group='Gerstner 5' min=-1.0 max=1.0 step=0.01");
			TwAddVarRW(tweakBar, "DirectionZAxis5", TW_TYPE_FLOAT, &dir_z[4], " label='Direction z axis' group='Gerstner 5' min=-1.0 max=1.0 step=0.01");
			TwAddVarRW(tweakBar, "Steepness5", TW_TYPE_FLOAT, &steepness[4], " label='Steepness' group='Gerstner 5' min=0.0 max=250.0 step=0.1");
			
			TwAddVarRW(tweakBar, "Wavelength6", TW_TYPE_FLOAT, &wavelength[5], " label='Wavelength' group='Gerstner 6' min=0.1 max=500.0 step=0.1");
			TwAddVarRW(tweakBar, "Amplitude6", TW_TYPE_FLOAT, &amplitude[5], " label='Amplitude' group='Gerstner 6' min=0.01 max=7.0 step=0.1");
			TwAddVarRW(tweakBar, "Speed6", TW_TYPE_FLOAT, &speed[5], " label='Speed' group='Gerstner 6' min=0.01 max=5.0 step=0.1");
			TwAddVarRW(tweakBar, "DirectionXAxis6", TW_TYPE_FLOAT, &dir_x[5], " label='Direction x axis' group='Gerstner 6' min=-1.0 max=1.0 step=0.01");
			TwAddVarRW(tweakBar, "DirectionZAxis6", TW_TYPE_FLOAT, &dir_z[5], " label='Direction z axis' group='Gerstner 6' min=-1.0 max=1.0 step=0.01");
			TwAddVarRW(tweakBar, "Steepness6", TW_TYPE_FLOAT, &steepness[5], " label='Steepness' group='Gerstner 6' min=0.0 max=250.0 step=0.1");
			
			TwAddVarRW(tweakBar, "Wavelength7", TW_TYPE_FLOAT, &wavelength[6], " label='Wavelength' group='Gerstner 7' min=0.1 max=500.0 step=0.1");
			TwAddVarRW(tweakBar, "Amplitude7", TW_TYPE_FLOAT, &amplitude[6], " label='Amplitude' group='Gerstner 7' min=0.01 max=7.0 step=0.1");
			TwAddVarRW(tweakBar, "Speed7", TW_TYPE_FLOAT, &speed[6], " label='Speed' group='Gerstner 7' min=0.01 max=5.0 step=0.1");
			TwAddVarRW(tweakBar, "DirectionXAxis7", TW_TYPE_FLOAT, &dir_x[6], " label='Direction x axis' group='Gerstner 7' min=-1.0 max=1.0 step=0.01");
			TwAddVarRW(tweakBar, "DirectionZAxis7", TW_TYPE_FLOAT, &dir_z[6], " label='Direction z axis' group='Gerstner 7' min=-1.0 max=1.0 step=0.01");
			TwAddVarRW(tweakBar, "Steepness7", TW_TYPE_FLOAT, &steepness[6], " label='Steepness' group='Gerstner 7' min=0.0 max=250.0 step=0.1");
			
			TwAddVarRW(tweakBar, "Wavelength8", TW_TYPE_FLOAT, &wavelength[7], " label='Wavelength' group='Gerstner 8' min=0.1 max=500.0 step=0.1");
			TwAddVarRW(tweakBar, "Amplitude8", TW_TYPE_FLOAT, &amplitude[7], " label='Amplitude' group='Gerstner 8' min=0.01 max=7.0 step=0.1");
			TwAddVarRW(tweakBar, "Speed8", TW_TYPE_FLOAT, &speed[7], " label='Speed' group='Gerstner 8' min=0.01 max=5.0 step=0.1");
			TwAddVarRW(tweakBar, "DirectionXAxis8", TW_TYPE_FLOAT, &dir_x[7], " label='Direction x axis' group='Gerstner 8' min=-1.0 max=1.0 step=0.01");
			TwAddVarRW(tweakBar, "DirectionZAxis8", TW_TYPE_FLOAT, &dir_z[7], " label='Direction z axis' group='Gerstner 8' min=-1.0 max=1.0 step=0.01");
			TwAddVarRW(tweakBar, "Steepness8", TW_TYPE_FLOAT, &steepness[7], " label='Steepness' group='Gerstner 8' min=0.0 max=250.0 step=0.1");

#pragma endregion		

		}

		void initialize_waves()
		{
			_totalSteepness = rand.get(0.0f, 1.0f);
			oceancolor = vec3(0.1f, 0.2f, 1.0f);
			n_waves = 8;
			for (size_t i = 0; i < n_waves; ++i)
			{
				OceanTerrainMesh::GerstnerWave* gw = new OceanTerrainMesh::GerstnerWave(rand.get(0.1f, dimension), rand.get(0.1f, 5.0f), rand.get(0.01f, 3.0f), vec3(rand.get(-1.0f, 1.0f), 0.0f, rand.get(-1.0f, 1.0f)).normalize(), _totalSteepness);
				waves.push_back(*gw);
			}

		}
		
		void update_uniform_variables()
		{
			for (size_t i = 0; i != 8; ++i)
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
			water_material->set_uniform(uniform_n_waves, &n_waves, sizeof(int));
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
			//float terrainsize = terrainmesh->get_aabb().get_max().length();
			for (size_t i = 0; i != 8; i++)
			{
				if(i < n_waves)
				{
					waves[i].wavelength = rand.get(0.01f, 0.5f * dimension);
					waves[i].amplitude = rand.get(0.1f, 6.0f);
					waves[i].speed = rand.get(0.01f, 2.0f);
					waves[i].direction = vec3(rand.get(-1.0f, 1.0f), 0.0f, rand.get(-1.0f, 1.0f)).normalize();
					waves[i].frequency = 2 * PI / waves[i].wavelength;
					waves[i].phase = waves[i].speed * waves[i].frequency;
					waves[i].steepness = _totalSteepness / (n_waves * waves[i].frequency * waves[i].amplitude);
				}
				else
				{
					waves[i].wavelength = 0.01f;
					waves[i].amplitude = 0.0f;
					waves[i].speed = 0.0f;
					waves[i].direction = vec3(0.0f, 0.0f, 0.0f);
					waves[i].frequency = 0.0f;
					waves[i].phase = 0.0f;
					waves[i].steepness = 0.0f;
				}
			}
			update_uniform_variables();
			update_uniforms();
		}

		void keyboard(){
			if (is_key_down(key::key_esc))
			{
				exit(1);
			}
			if (is_key_going_down('1')){
				terrainmesh->set_mode(1);
			}
			else if (is_key_going_down('2')){
				terrainmesh->set_mode(4);
			}
			else if (is_key_going_down('3')){
				terrainmesh->set_mode(0);
			}
			if (is_key_down('W')){
				camera->get_node()->translate(vec3(0, 0, -5));
			}
			if (is_key_down('S')){
				camera->get_node()->translate(vec3(0, 0, 5));
			}
			if (is_key_down('A')){
				camera->get_node()->translate(vec3(-5, 0, 0));
			}
			if (is_key_down('D')){
				camera->get_node()->translate(vec3(5, 0, 0));
			}
			if (is_key_down('E')){
				camera->get_node()->translate(vec3(0, 5, 0));
			}
			if (is_key_down('Q')){
				camera->get_node()->translate(vec3(0, -5, 0));
			}
			//new generation
			if (is_key_down('G')){
				GenerateNewWaveSet();
			}
		}

		void mouse(){

			if (is_key_going_down(key_shift))
			{
				mouse_look_helper.reset_mouse_center();
			}
			if (is_key_down(key_shift))
			{
				scene_node *camera_node = camera->get_node();
				mat4t &camera_to_world = camera_node->access_nodeToParent();
				mouse_look_helper.update(camera_to_world);
			}
			if (is_key_going_up(key_shift))
			{
				int mousex, mousey;
				get_absolute_mouse_movement(mousex, mousey);
				mouse_look_helper.set_mouse_center(mousex, mousey);
			}	

			//Updating AntTweak
			int mX = 0, mY = 0;
			get_mouse_pos(mX, mY);
			TwMouseMotion(mX, mY);

			
			if (is_key_going_down(key_lmb))
			{
				TwMouseButton(TW_MOUSE_PRESSED, TW_MOUSE_LEFT);
				update_uniforms();
			}
			else if (is_key_down(key_lmb))
			{
				update_uniforms();
			}
			if (!is_key_down(key_lmb) && get_prev_keys()[key_lmb] != 0)
			{
				TwMouseButton(TW_MOUSE_RELEASED, TW_MOUSE_LEFT);
				update_uniforms();
			}

		}

		void add_light_instances()
		{
			scene_node *node = new scene_node();
			app_scene->add_child(node);
			light *_light = new light();
			light_instance *li = new light_instance();
			node->translate(vec3(50, 50, 50));
			node->rotate(-45, vec3(1, 0, 0));
			node->rotate(45, vec3(0, 1, 0));
			_light->set_color(vec4(1, 1, 1, 1));
			_light->set_kind(atom_directional);
			li->set_node(node);
			li->set_light(_light);
			app_scene->add_light_instance(li);

			//node = new scene_node();
			//app_scene->add_child(node);
			//_light = new light();
			//li = new light_instance();
			//node->translate(vec3(-100, 100, -100));
			//node->rotate(45, vec3(1, 0, 0));
			//node->rotate(-45, vec3(0, 1, 0));
			//_light->set_color(vec4(1, 1, 1, 1));
			//_light->set_kind(atom_directional);
			//li->set_node(node);
			//li->set_light(_light);
			//app_scene->add_light_instance(li);

			//DIFFUSE LIGHT
			//node = new scene_node();
			//app_scene->add_child(node);
			//_light = new light();
			//node->rotate(-90, vec3(1, 0, 0));
			//node->translate(vec3(0.0f, 0.0f, 0.0f));
			//_light->set_color(vec4(1, 1, 1, 1));
			//_light->set_kind(atom_diffuse_light);
			//li->set_node(node);
			//li->set_light(_light);
			//app_scene->add_light_instance(li);
		}

		void tweakbar_buttons()
		{
			if (doRandom){
				GenerateNewWaveSet();
				doRandom = false;
			}
			water_material->set_diffuse(vec4(oceancolor,1.0f));
		}

	public:
		/// this is called when we construct the class before everything is initialised.
		OceanWaterGLSL(int argc, char **argv) : app(argc, argv) {
		}

		/// this is called once OpenGL is initialized
		void app_init() {

			mouse_look_helper.init(this, 200.0f / 360.0f, false);
			app_scene = new visual_scene();
			add_light_instances();
			app_scene->create_default_camera_and_lights();
			camera = app_scene->get_camera_instance(0);
			//camera->get_node()->access_nodeToParent().rotate(210,0, 1, 0);
			//camera->get_node()->translate(vec3(-300, 200, -200));
			mat4t& cameraToWorld = camera->get_node()->access_nodeToParent();
			cameraToWorld.x() = vec4(1, 0, 0, 0);
			cameraToWorld.y() = vec4(0, 1, 0, 0);
			cameraToWorld.z() = vec4(0, 0, 1, 0);
			cameraToWorld.rotateY(70.0f);
			//cameraToWorld.rotateX(30.0f);
			camera->set_far_plane(10000);		

			mat4t mat;
			mat.loadIdentity();
			//mat.translate(50, 0, 50);

			initialize_waves();
			update_uniform_variables();		

			initializes_bar();

			//param_shader* water_shader = new param_shader("shaders/basewater.vs", "shaders/textured_shader.fs");
			//water_material = new material(new image("assets/watertexture/sea.gif"), 0, water_shader);
			param_shader* water_shader = new param_shader("shaders/basewater.vs", "shaders/base_shader.fs");
			water_material = new material(vec4(oceancolor, 1.0f), water_shader);
			register_uniforms();
			update_uniforms();

			terrainmesh = new mesh_terrain(vec3(dimension, 0.0f, dimension), ivec3(precision, 1, precision), water_source);

			app_scene->add_shape(
				mat,
				terrainmesh,
				water_material,
				false, 0
				);

			if (skybox)
			{
				scene_node* skynode = new scene_node();
				skynode->rotate(180, vec3(0.0f, 1.0f, 0.0f));
				skynode->rotate(20, vec3(1.0f, 0.0f, 0.0f));
				app_scene->add_mesh_instance(new mesh_instance(skynode, new mesh_sphere(vec3(0, 0, 0), 4000), new material(new image("assets/skydome/skybox2.jpg"))));
			}
		}

		/// this is called to draw the world
		void draw_world(int x, int y, int w, int h) {
			
			int vx = 0, vy = 0;
			get_viewport_size(vx, vy);
			TwWindowSize(vx, vy);
			app_scene->begin_render(vx, vy, vec4(0.0f,0.0f,0.0f,1.0f));

			keyboard();
			mouse();
			
			tweakbar_buttons();

			//glClearColor(0, 0, 0, 1);
			//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			water_material->set_uniform(uniform_time, &update_time, sizeof(update_time));

			// update matrices. assume 30 fps.
			app_scene->update(1.0f / 30);
			
			// draw the scene
			app_scene->render((float)vx / vy);
			TwDraw();
			update_time += 1.0f;
		}
	};

}

