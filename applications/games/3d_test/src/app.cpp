#include "sys.h"
#include "stdio.hpp"
#include "app.hpp"
#include "stdlib.h"
#include "classes.hpp"
#include "app/parsing/image_read.h"

#define COLOR_BG				(Color(0, 0, 0).value)
#define COLOR_EDGE				(Color(255, 255, 255).value)
#define COLOR_VERTEX			(Color(255, 0, 0).value)

#ifndef APP_KEY_ARROW_LEFT
#define APP_KEY_ARROW_LEFT		0x102
#define APP_KEY_ARROW_RIGHT		0x103
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
	point.visible = true; //point.x >= 0 && point.x < display.w && point.y >= 0 && point.y < display.h;
	return point;
}

static void draw_edge(Display &display, const ScreenPoint &start, const ScreenPoint &end, uint32_t override_color = 0)
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
		display.put_pixel(x, y, override_color?override_color:COLOR_EDGE);
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

static void draw_textured_face(Display &display, const Object &object, const Face &face,
	const Camera &camera, const Texture &texture)
{
	ScreenPoint p[3];
	for (int i = 0; i < 3; ++i)
	{
		if (face.vertices[i] >= object.vertices_count)
			return;
		
		p[i] = project_vertex(object.vertices[face.vertices[i]].point, camera, display);
		if (!p[i].visible)
			return;
	}
	float area = (float)(p[1].x - p[0].x) * (p[2].y - p[0].y) - (float)(p[1].y - p[0].y) * (p[2].x - p[0].x);
	if (area == 0)
		return;
	
	int min_x = p[0].x, max_x = p[0].x, min_y = p[0].y, max_y = p[0].y;
	for (int i = 1; i < 3; ++i)
	{
		if (p[i].x < min_x)
			min_x = p[i].x;
		if (p[i].x > max_x)
			max_x = p[i].x;
		if (p[i].y < min_y)
			min_y = p[i].y;
		if (p[i].y > max_y)
			max_y = p[i].y;
	}
	if (min_x < 0)
		min_x = 0;
	if (min_y < 0)
		min_y = 0;
	
	if (max_x >= display.w)
		max_x = display.w - 1;
	if (max_y >= display.h)
		max_y = display.h - 1;
	
	for (int y = min_y; y <= max_y; ++y)
		for (int x = min_x; x <= max_x; ++x)
		{
			float w1 = ((float)(x - p[0].x) * (p[2].y - p[0].y) - (float)(y - p[0].y) * (p[2].x - p[0].x)) / area;
			float w2 = ((float)(p[1].x - p[0].x) * (y - p[0].y) - (float)(p[1].y - p[0].y) * (x - p[0].x)) / area;
			float w0 = 1.0f - w1 - w2;
			if (w0 < 0 || w1 < 0 || w2 < 0)
				continue;
			float u = w0 * face.uv[0].u + w1 * face.uv[1].u + w2 * face.uv[2].u;
			float v = w0 * face.uv[0].v + w1 * face.uv[1].v + w2 * face.uv[2].v;
			display.put_pixel(x, y, texture.sample(u, v));
		}
}


ScreenPoint	vertical_intersection_point(Display &display, ScreenPoint *p1, ScreenPoint *p2, int x)
{
	if ((p1->x < x && p2->x < x && x <= 0) || (p1->x > x && p2->x > x && x >= display.w))
		return (ScreenPoint){0, 0, false};

	if (p1->x == p2->x)
		return (ScreenPoint){x, p1->y, false};

	double ratio = (double)(x - p1->x) / (p2->x - p1->x);
	int y = p1->y + ratio * (p2->y - p1->y);

	return (ScreenPoint){x, y, true};
}


static void draw_object(Display &display, const Object &object, const Face *faces, size_t face_count,
	const Camera &camera, const Texture &texture, uint32_t override_color = 0)
{
	if (!override_color)
		for (size_t i = 0; i < face_count; ++i)
			draw_textured_face(display, object, faces[i], camera, texture);
	for (size_t i = 0; i < object.edges_count; ++i)
	{
		const Edge &edge = object.edges[i];
		if (edge.start >= object.vertices_count || edge.end >= object.vertices_count)
			continue;
		
		ScreenPoint a = project_vertex(object.vertices[edge.start].point, camera, display);
		ScreenPoint b = project_vertex(object.vertices[edge.end].point, camera, display);
		if (!a.visible)
			continue;

		if (!b.visible)
		{
			ScreenPoint inter = vertical_intersection_point(display, &a, &b, (a.x - b.x > 0)?0:display.w);
			if (inter.visible)
				draw_edge(display, a, inter, override_color);
		}
		else
		{
			if (b.visible)
				draw_edge(display, a, b, override_color);
		}
	}

	for (size_t i = 0; i < object.vertices_count; ++i)
	{
		ScreenPoint point = project_vertex(object.vertices[i].point, camera, display);
		if (point.visible)
			display.put_pixel(point.x, point.y, override_color?override_color:COLOR_VERTEX);
	}
}

static void render_scene(Display &display, const Object &object, const Face *faces, size_t face_count,
	const Camera &camera, const Texture &texture)
{
	display.draw_rect(0, 0, display.w, display.h, COLOR_BG);
	draw_object(display, object, faces, face_count, camera, texture);
	display.present();
	//draw_object(display, object, faces, face_count, camera, texture, COLOR_BG);
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
		case 27: // esc
			exit(0);
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
	const Face faces[] = {
		{ { 0, 1, 2 }, { Vec2(0.5f, 1.0f), Vec2(0.0f, 0.0f), Vec2(1.0f, 0.0f) } },
		{ { 0, 2, 3 }, { Vec2(0.5f, 1.0f), Vec2(0.0f, 0.0f), Vec2(1.0f, 0.0f) } },
		{ { 0, 3, 4 }, { Vec2(0.5f, 1.0f), Vec2(0.0f, 0.0f), Vec2(1.0f, 0.0f) } },
		{ { 0, 4, 1 }, { Vec2(0.5f, 1.0f), Vec2(0.0f, 0.0f), Vec2(1.0f, 0.0f) } },
		{ { 1, 4, 3 }, { Vec2(0.0f, 0.0f), Vec2(0.0f, 1.0f), Vec2(1.0f, 1.0f) } },
		{ { 1, 3, 2 }, { Vec2(0.0f, 0.0f), Vec2(1.0f, 1.0f), Vec2(1.0f, 0.0f) } }
	};

	Object pyramid(vertices, sizeof(vertices) / sizeof(vertices[0]), edges, sizeof(edges) / sizeof(edges[0]));
	Texture texture;
	if (!texture.load("/games/textures/texture1.png"))
		fprintf(stderr, "Could not load textures; using fallback color\n");
	render_scene(display, pyramid, faces, sizeof(faces) / sizeof(faces[0]), camera, texture);

	while (1)
	{
		event_t event = { .type = EVENT_UNKNOWN, .timestamp = 0, .x = 0, .y = 0, .key = 0, .modifiers = 0 };
		if (!evtpop(&event))
			continue;
		if (event.type == EVENT_KEYPRESS)
			move_camera(camera, event.key);
		render_scene(display, pyramid, faces, sizeof(faces) / sizeof(faces[0]), camera, texture);
	}
}
