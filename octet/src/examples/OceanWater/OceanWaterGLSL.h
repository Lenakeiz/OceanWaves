#include "OceanTerrainMesh.h"

namespace octet
{
	class OceanWaterGLSL : public app
	{
		// scene for drawing box
		ref<visual_scene> app_scene;

		ref<material> water_material;
		ref<param_uniform> uniform_time;

		std::chrono::time_point<std::chrono::system_clock> start;
		float time_per_frame;

		mouse_look mouse_look_helper;
		ref<camera_instance> the_camera;

		OceanTerrainMesh::water_geometry_source water_source;

		void keyboard(){
			if (is_key_down(key::key_esc))
			{
				exit(1);
			}
			else if (is_key_going_down('1')){
				//om->SetMode(1);
			}
			else if (is_key_going_down('2')){
				//om->SetMode(5);
			}
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
				the_camera->get_node()->translate(vec3(0, 0, -5));
			}
			else if (is_key_down('S')){
				the_camera->get_node()->translate(vec3(0, 0, 5));
			}
			else if (is_key_down('A')){
				the_camera->get_node()->translate(vec3(-5, 0, 0));
			}
			else if (is_key_down('D')){
				the_camera->get_node()->translate(vec3(5, 0, 0));
			}
			//new generation
			//else if (is_key_down('G')){
			//	om->GenerateNewWaveSet();
			//}
		}

	public:
		/// this is called when we construct the class before everything is initialised.
		OceanWaterGLSL(int argc, char **argv) : app(argc, argv) {
		}

		/// this is called once OpenGL is initialized
		void app_init() {

			start = std::chrono::system_clock::now();
			time_per_frame = 20.0f;

			mouse_look_helper.init(this, 200.0f / 360.0f, false);
			app_scene = new visual_scene();
			app_scene->create_default_camera_and_lights();
			the_camera = app_scene->get_camera_instance(0);
			the_camera->get_node()->translate(vec3(0, 4, 0));
			the_camera->set_far_plane(10000);

			mat4t mat;

			mat.loadIdentity();
			mat.translate(0, -0.5f, 0);

			param_shader* water_shader = new param_shader("shaders/default_solid.vs", "shaders/default_solid.fs");
			water_material = new material(vec4(0.0f, 0.0f, 1.0f, 1.0f), water_shader);

			atom_t atom_my_time = app_utils::get_atom("time");

			float time_value = 0;
			uniform_time = water_material->add_uniform(&time_value, atom_my_time, GL_FLOAT, 1, param::stage_vertex);

			app_scene->add_shape(
				mat,
				new mesh_terrain(vec3(50.0f, 0.5f, 50.0f), ivec3(50, 1, 50), water_source),
				water_material,
				false, 0
				);

			float player_height = 1.83f;
			float player_radius = 0.25f;
			float player_mass = 90.0f;

			mat.loadIdentity();
			mat.translate(0, player_height*5.0f, -50);

			mesh_instance *mi = app_scene->add_shape(
				mat,
				new mesh_sphere(vec3(0), player_radius),
				new material(vec4(0, 0, 1, 1)),
				true, player_mass,
				new btCapsuleShape(0.25f, player_height)
				);
		}

		/// this is called to draw the world
		void draw_world(int x, int y, int w, int h) {
			int vx = 0, vy = 0;
			get_viewport_size(vx, vy);
			app_scene->begin_render(vx, vy);

			scene_node *camera_node = the_camera->get_node();
			mat4t &camera_to_world = camera_node->access_nodeToParent();
			mouse_look_helper.update(camera_to_world);

			std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
			std::chrono::duration<float> elapsed_seconds = now - start;
			float new_time = elapsed_seconds.count();
			water_material->set_uniform(uniform_time, &new_time, sizeof(new_time));

			// update matrices. assume 30 fps.
			app_scene->update(1.0f / 30);

			// draw the scene
			app_scene->render((float)vx / vy);
		}
	};

}

