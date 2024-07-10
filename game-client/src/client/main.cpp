#include <cstdlib>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "afront/advancing_front.h"
#include "client/volume/csg.h"
#include "core/debug/log.h"
#include "render/mesh_view.h"
#include "render/render_util.h"
#include "render/shader/game_shaders.h"
#include "render/shader/game_shader_settings.h"
#include "util/keyboard_control.h"
#include "util/keyboard_input.h"
#include "util/relative_camera.h"
#include "util/ticker.h"

#include "entity.h"
#include "example_models.h"

namespace {
playchilla::keyboard_input& get_keyboard_input() {
	static playchilla::keyboard_input ki;
	return ki;
}

void glfw_error_callback(int error, const char* description) {
	playchilla::logger() << "Error: " << description << " (" << error << ")\n";
}

void glfw_key_callback(GLFWwindow* window, int key, int scan_code, int action, int mods) {
	if (action == GLFW_PRESS) {
		get_keyboard_input().press(key);
	}
	else if (action == GLFW_RELEASE) {
		// note: pressing fn key on mac generates a release event with key == -1
		if (key != -1) {
			get_keyboard_input().release(key);
		}
	}
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
}

void start_demo(GLFWwindow* window) {
	using namespace playchilla;

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);


	transform camera_transform;
	relative_camera camera(width, height, 0.2, 1000000., 67);
	keyboard_control camera_control(&get_keyboard_input(), &camera_transform);

	const auto shader_map = create_shaders();

	csg csg(1234);
	surface_entity sphere(csg.sphere(20).get(), &camera_transform);
	surface_entity cube(csg.cube({30, 30, 30}).get(), &camera_transform);
	surface_entity sphere_cube(csg.unions(
		                           {
			                           csg.sphere(10),
			                           csg.cube({30, 10, 10,})
		                           }),
	                           &camera_transform);
	surface_entity sphere_hole(csg.differences(
		                           csg.sphere(10),
		                           {csg.cube({30, 10, 10})}
	                           ),
	                           &camera_transform);

	surface_entity noisy_terrain(create_planet(csg, 100), &camera_transform, 2.0);

	cube.get_view()->get_transform().set_pos({-60, 0, 0});
	sphere_cube.get_view()->get_transform().set_pos({60, 0, 0});
	sphere_hole.get_view()->get_transform().set_pos({100, 0, 0});
	noisy_terrain.get_view()->get_transform().set_pos(0, 0, -150);

	ticker ticker(60, [&](const tick_data& tick) {
		sphere.on_tick(tick);
		cube.on_tick(tick);
		sphere_cube.on_tick(tick);
		sphere_hole.on_tick(tick);
		noisy_terrain.on_tick(tick);

		camera_control.on_tick(tick);
		camera.update(camera_transform.get_pos(), camera_transform.get_up(), camera_transform.get_forward());
		get_keyboard_input().reset();
	});

	shader_environment shader_env{&camera, {}, &shader_map};
	std::vector<const mesh_view*> mesh_views;
	mesh_views.push_back(sphere.get_view());
	mesh_views.push_back(cube.get_view());
	mesh_views.push_back(sphere_cube.get_view());
	mesh_views.push_back(sphere_hole.get_view());
	mesh_views.push_back(noisy_terrain.get_view());

	int fps = 0;
	timer t;
	while (!glfwWindowShouldClose(window)) {
		++fps;
		glfwGetFramebufferSize(window, &width, &height);
		gl_check(glViewport(0, 0, width, height));
		gl_check(glClearStencil(0xff));
		gl_check(glClearColor(0, 0, 0, 0));
		gl_check(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));

		ticker.step();

		render_meshes(shader_env, mesh_views);

		glfwSwapBuffers(window);
		glfwPollEvents();

		if (t.seconds() >= 1) {
			glfwSetWindowTitle(window, ("demo at " + std::to_string(fps) + " fps").c_str());
			fps = 0;
			t.reset();
		}
	}
}
}

int main() {
	glfwSetErrorCallback(glfw_error_callback);

	if (!glfwInit()) {
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	GLFWwindow* window = glfwCreateWindow(1024, 768, "Advancing front triangulation", nullptr, nullptr);
	if (!window) {
		glfwTerminate();
		return -1;
	}

	glfwSetKeyCallback(window, glfw_key_callback);

	glfwMakeContextCurrent(window);
	gladLoadGL();
	glfwSwapInterval(0);

	start_demo(window);

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
