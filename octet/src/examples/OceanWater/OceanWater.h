////////////////////////////////////////////////////////////////////////////////
//
// 
//
// 
//

#include "OceanMesh.h"

namespace octet {
  /// Scene containing a box with octet.
  class OceanWater : public app {
    // scene for drawing box
    ref<visual_scene> app_scene;
	mouse_look mouse_look_helper;

	ref<camera_instance> camera;

	OceanMesh* om;
  public:
    /// this is called when we construct the class before everything is initialised.
    OceanWater(int argc, char **argv) : app(argc, argv) {
    }

    /// this is called once OpenGL is initialized
	void app_init() {
		mouse_look_helper.init(this, 200.0f / 360.0f, false);
		om = new OceanMesh();
		om->Init();
		app_scene =  new visual_scene();
		app_scene->create_default_camera_and_lights();

		camera = app_scene->get_camera_instance(0);
		camera->get_node()->translate(vec3(0, 4, 0));
		camera->set_far_plane(10000);

		mat4t mat;

		mat.loadIdentity();
		mat.translate(0, -0.5f, 0);
		material *color = new material(vec4(0, 0, 1, 0.5f));
		scene_node *node = new scene_node();
		
		app_scene->add_child(node);
		app_scene->add_mesh_instance(new mesh_instance(node, om->getMeshInstance(), color));

	}

    /// this is called to draw the world
    void draw_world(int x, int y, int w, int h) {
      int vx = 0, vy = 0;
      get_viewport_size(vx, vy);
      app_scene->begin_render(vx, vy);

	  scene_node *camera_node = camera->get_node();
	  mat4t &camera_to_world = camera_node->access_nodeToParent();
	  mouse_look_helper.update(camera_to_world);

      // update matrices. assume 30 fps.
      app_scene->update(1.0f/30);

      // draw the scene
      app_scene->render((float)vx / vy);

      // tumble the box  (there is only one mesh instance)
      scene_node *node = app_scene->get_mesh_instance(0)->get_node();
      //node->rotate(1, vec3(1, 0, 0));
      //node->rotate(1, vec3(0, 1, 0));
    }
  };
}
