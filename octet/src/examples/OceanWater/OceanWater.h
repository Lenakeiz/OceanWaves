////////////////////////////////////////////////////////////////////////////////
//
// Andrea Castegnaro
//
// 
//

#include "OceanMesh.h"
#include <chrono>
#include <ctime>

namespace octet {
  /// Scene containing a box with octet.
  class OceanWater : public app {
    // scene for drawing box
    ref<visual_scene> app_scene;
	mouse_look mouse_look_helper;

	ref<camera_instance> camera;

	OceanMesh* om;

	std::chrono::time_point<std::chrono::system_clock> start;

  public:
    /// this is called when we construct the class before everything is initialised.
    OceanWater(int argc, char **argv) : app(argc, argv) {
    }

	void keyboard(){
		if (is_key_down(key::key_esc))
		{
			exit(1);
		}
		else if (is_key_going_down('1')){
			om->SetMode(1);
		}
		else if (is_key_going_down('2')){
			om->SetMode(5);
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
		//new generation
		else if (is_key_down('G')){
			om->GenerateNewWaveSet();
		}
	}

	void mouse(){
		if (is_key_down(key::key_shift))
		{
			scene_node *camera_node = camera->get_node();
			mat4t &camera_to_world = camera_node->access_nodeToParent();
			mouse_look_helper.update(camera_to_world);
		}		
	}
	
    /// this is called once OpenGL is initialized
	void app_init() {
		
		mouse_look_helper.init(this, 200.0f / 360.0f, false);
		app_scene =  new visual_scene();

		light *l = new light();
		light_instance *li = new light_instance();
		scene_node *n = new scene_node();
		app_scene->add_child(n);
		n->translate(vec3(0.0f, 50, -50));
		n->rotate(-45, vec3(1, 0, 0));
		//n->rotate(-180, vec3(0, 1, 0));
		l->set_color(vec4(1, 1, 1, 1));
		l->set_kind(atom_directional);
		li->set_node(n);
		li->set_light(l);
		app_scene->add_light_instance(li);

		app_scene->create_default_camera_and_lights();

		camera = app_scene->get_camera_instance(0);
		camera->get_node()->translate(vec3(0, 4, 0));
		camera->set_far_plane(10000);

		start = std::chrono::system_clock::now();

		om = new OceanMesh();
		om->Init(app_scene);
	}

    /// this is called to draw the world
    void draw_world(int x, int y, int w, int h) {
		
		keyboard();
		mouse();

		int vx = 0, vy = 0;
		get_viewport_size(vx, vy);
		app_scene->begin_render(vx, vy);

		scene_node *camera_node = camera->get_node();
		mat4t &camera_to_world = camera_node->access_nodeToParent();
		mouse_look_helper.update(camera_to_world);

		om->FixedUpdate();
		
		// update matrices. assume 30 fps.
		app_scene->update(1.0f/30);

		// draw the scene
		app_scene->render((float)vx / vy);

		}
	};
}
