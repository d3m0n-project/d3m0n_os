#include "sys.h"
#include "stdio.hpp"
#include "app.hpp"
#include "stdlib.h"
#include "classes.hpp"

#define COLOR_BG          (Color(0, 0, 0).value)
#define COLOR_EDGE        (Color(130, 205, 255).value)
#define COLOR_VERTEX      (Color(255, 255, 255).value)

#ifndef APP_KEY_ARROW_LEFT
#define APP_KEY_ARROW_LEFT  0x102
#define APP_KEY_ARROW_RIGHT 0x103
#endif

static const float SIN_TABLE[24] = {
	0.0f, 0.258819f, 0.5f, 0.707107f, 0.866025f, 0.965926f,
	1.0f, 0.965926f, 0.866025f, 0.707107f, 0.5f, 0.258819f,
	0.0f, -0.258819f, -0.5f, -0.707107f, -0.866025f, -0.965926f,
	-1.0f, -0.965926f, -0.866025f, -0.707107f, -0.5f, -0.258819f
};

static float camera_sine(const Camera &camera)
{
	return SIN_TABLE[camera.yaw_step];
}

static float camera_cosine(const Camera &camera)
{
	return SIN_TABLE[(camera.yaw_step + 6) % 24];
}

struct ScreenPoint
{
	int x;
	int y;
	bool visible;
};

static ScreenPoint project_vertex(const Vec3 &vertex, const Camera &camera, const Display &display)
{
	Vec3 relative = vertex - camera.position;
	float cosine = camera_cosine(camera);
	float sine = camera_sine(camera);
	float camera_x = relative.x * cosine - relative.z * sine;
	float camera_z = relative.x * sine + relative.z * cosine;
	ScreenPoint point = { 0, 0, false };
	if (camera_z <= 0.1f)
		return point;

	point.x = (int)(display.w / 2.0f + (camera_x / camera_z) * camera.focal_length);
	point.y = (int)(display.h / 2.0f - (relative.y / camera_z) * camera.focal_length);
	point.visible = point.x >= 0 && point.x < display.w && point.y >= 0 && point.y < display.h;
	return point;
}

static void draw_edge(Display &display, const ScreenPoint &start, const ScreenPoint &end)
{
	int x = start.x;
	int y = start.y;
	int dx = end.x >= x ? end.x - x : x - end.x;
	int dy = end.y >= y ? end.y - y : y - end.y;
	int step_x = x < end.x ? 1 : -1;
	int step_y = y < end.y ? 1 : -1;
	int error = dx - dy;
	int limit = display.w + display.h + 1;

	for (int i = 0; i < limit; ++i)
	{
		display.put_pixel(x, y, COLOR_EDGE);
		if (x == end.x && y == end.y)
			return;
		int doubled_error = 2 * error;
		if (doubled_error > -dy)
		{
			error -= dy;
			x += step_x;
		}
		if (doubled_error < dx)
		{
			error += dx;
			y += step_y;
		}
	}
}

static void draw_object(Display &display, const Object &object, const Camera &camera)
{
	for (size_t i = 0; i < object.edges_count; ++i)
	{
		const Edge &edge = object.edges[i];
		if (edge.start >= object.vertices_count || edge.end >= object.vertices_count)
			continue;
		ScreenPoint a = project_vertex(object.vertices[edge.start].point, camera, display);
		ScreenPoint b = project_vertex(object.vertices[edge.end].point, camera, display);
		if (a.visible && b.visible)
			draw_edge(display, a, b);
	}

	for (size_t i = 0; i < object.vertices_count; ++i)
	{
		ScreenPoint point = project_vertex(object.vertices[i].point, camera, display);
		if (point.visible)
			display.put_pixel(point.x, point.y, COLOR_VERTEX);
	}
}

static void render_scene(Display &display, const Object &object, const Camera &camera)
{
	display.draw_rect(0, 0, display.w, display.h, COLOR_BG);
	draw_object(display, object, camera);
	display.present();
}

static void move_camera(Camera &camera, int key)
{
	const float move_step = 0.5f;
	float forward_x = camera_sine(camera);
	float forward_z = camera_cosine(camera);
	float right_x = camera_cosine(camera);
	float right_z = -camera_sine(camera);

	switch (key)
	{
		case 'w':
			camera.position.x += forward_x * move_step;
			camera.position.z += forward_z * move_step;
			break;
		case 's':
			camera.position.x -= forward_x * move_step;
			camera.position.z -= forward_z * move_step;
			break;
		case 'a':
			camera.position.x -= right_x * move_step;
			camera.position.z -= right_z * move_step;
			break;
		case 'd':
			camera.position.x += right_x * move_step;
			camera.position.z += right_z * move_step;
			break;
		case APP_KEY_ARROW_LEFT:
			camera.yaw_step = (camera.yaw_step + 23) % 24;
			break;
		case APP_KEY_ARROW_RIGHT:
			camera.yaw_step = (camera.yaw_step + 1) % 24;
			break;
	}
}

void game_start(void)
{
	Display display;
	Camera camera(Vec3(0.0f, 0.0f, 0.0f), 300.0f);

	const Vertex vertices[] = {
		{ Vec3( 0.0f,  2.0f, 8.0f) },
		{ Vec3(-2.0f, -1.0f, 7.0f) },
		{ Vec3( 2.0f, -1.0f, 7.0f) },
		{ Vec3( 2.0f, -1.0f, 9.0f) },
		{ Vec3(-2.0f, -1.0f, 9.0f) }
	};
	const Edge edges[] = {
		{ 0, 1 }, { 0, 2 }, { 0, 3 }, { 0, 4 },
		{ 1, 2 }, { 2, 3 }, { 3, 4 }, { 4, 1 }
	};

	Object pyramid(vertices, sizeof(vertices) / sizeof(vertices[0]), edges, sizeof(edges) / sizeof(edges[0]));
	render_scene(display, pyramid, camera);

	while (1)
	{
		event_t event = { .type = EVENT_UNKNOWN, .timestamp = 0, .x = 0, .y = 0, .key = 0, .modifiers = 0 };
		if (!evtpop(&event))
			continue;
		if (event.type == EVENT_KEYPRESS)
			move_camera(camera, event.key);
		render_scene(display, pyramid, camera);
	}
}
