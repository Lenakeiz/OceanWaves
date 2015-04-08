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

		OceanTerrainMesh::water_geometry_source water_source;
		OceanTerrainMesh::GerstnerWave* gw;
		ref<material> water_material;

		//uniforms sections: vertex shader
		ref<param_uniform> uniform_time;
		ref<param_uniform> uniform_wavelength;
		ref<param_uniform> uniform_amplitude;
		ref<param_uniform> uniform_speed;
		ref<param_uniform> uniform_dir_x;
		ref<param_uniform> uniform_dir_z;

		void initialize_waves()
		{
			gw = new OceanTerrainMesh::GerstnerWave(50.0f, 2.0f, 1.0f, vec3(1.0, 0.0f, 1.0f).normalize(), 0.0f);
		}

		void register_uniforms()
		{
			atom_t time_atom = app_utils::get_atom("time");
			uniform_time = water_material->add_uniform(&update_time, time_atom, GL_FLOAT, 1, param::stage_vertex);

			atom_t a_wavelength = app_utils::get_atom("wavelength");
			uniform_wavelength = water_material->add_uniform(&(gw->wavelength), a_wavelength, GL_FLOAT, 1, param::stage_vertex);

			atom_t a_amplitude = app_utils::get_atom("amplitude");
			uniform_amplitude = water_material->add_uniform(&(gw->amplitude), a_amplitude, GL_FLOAT, 1, param::stage_vertex);

			atom_t a_speed = app_utils::get_atom("speed");
			uniform_speed = water_material->add_uniform(&(gw->speed), a_speed, GL_FLOAT, 1, param::stage_vertex);

			atom_t a_dir_x = app_utils::get_atom("dir_x");
			uniform_dir_x = water_material->add_uniform(&(gw->direction.x()), a_dir_x, GL_FLOAT, 1, param::stage_vertex);

			atom_t a_dir_z = app_utils::get_atom("dir_z");
			uniform_dir_z = water_material->add_uniform(&(gw->direction.z()), a_dir_z, GL_FLOAT, 1, param::stage_vertex);

		}

		void keyboard(){
			if (is_key_down(key::key_esc))
			{
				exit(1);
			}
			/*else if (is_key_going_down('1')){
				om->SetMode(1);
			}
			else if (is_key_going_down('2')){
				om->SetMode(5);
			}*/
			/*else if (is_key_going_down('3')){
			om->SetMode(2);
			}
			else if (is_key_going_down('4')){
			om->SetMode(3);
			}
			else if (is_key_going_down('5')){
			om->SetMode(5);
			}*/
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
				camera->get_node()->translate(vec3(0, 5, 0));
			}
			//new generation
			/*else if (is_key_down('G')){
				om->GenerateNewWaveSet();
			}*/
		}

		void mouse(){
			if (is_key_down(key::key_shift))
			{
				scene_node *camera_node = camera->get_node();
				mat4t &camera_to_world = camera_node->access_nodeToParent();
				mouse_look_helper.update(camera_to_world);
			}
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

			mat4t mat;

			mat.loadIdentity();
			//mat.translate(0, -0.5f, 0);

			initialize_waves();

			param_shader* water_shader = new param_shader("shaders/basewater.vs", "shaders/default_solid.fs");
			water_material = new material(vec4(0.0f, 0.0f, 1.0f, 1.0f), water_shader);
			
			register_uniforms();
			
			app_scene->add_shape(
				mat,
				new mesh_terrain(vec3(100.0f, 0.0f, 100.0f), ivec3(400, 1, 400), water_source),
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

			water_material->set_uniform(uniform_time, &update_time, sizeof(update_time));

			// update matrices. assume 30 fps.
			app_scene->update(1.0f / 30);

			// draw the scene
			app_scene->render((float)vx / vy); 
			update_time += 0.2f;
		}
	};

}

