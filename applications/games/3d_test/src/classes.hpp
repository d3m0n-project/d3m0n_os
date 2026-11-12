#pragma once

class Vec3
{
public:
	float x;
	float y;
	float z;

	Vec3(float x, float y, float z) : x(x), y(y), z(z) {}
	Vec3 operator-(const Vec3 &other) const
	{
		return Vec3(x - other.x, y - other.y, z - other.z);
	}
};

class Camera
{
public:
	Vec3 position;
	float focal_length;
	int yaw_step;

	Camera(Vec3 position, float focal_length) : position(position), focal_length(focal_length), yaw_step(0) {}
};

struct Vertex
{
	Vec3 point;
};

struct Vec2
{
	float	u;
	float	v;
	Vec2(float u, float v) : u(u), v(v) {}
};

struct Face
{
	size_t	vertices[3];
	Vec2	uv[3];
};

struct Edge
{
	size_t	start;
	size_t	end;
};

class Object
{
public:
	const Vertex	*vertices;
	size_t			vertices_count;
	const			Edge *edges;
	size_t			edges_count;

	Object(const Vertex *vertex_list, size_t vertex_count, const Edge *edge_list, size_t edge_count) : vertices(vertex_list), vertices_count(vertex_count), edges(edge_list), edges_count(edge_count)
	{}
};

struct ScreenPoint
{
	int x;
	int y;
	bool visible;
};

struct Texture
{
	uint32_t *pixels;
	uint32_t width;
	uint32_t height;

	Texture() : pixels(0), width(0), height(0) {}
	~Texture() { if (pixels) free(pixels); }

	bool load(const char *path)
	{
		size_t size = 0;
		unsigned char *data = image_read_to_buf(path, &size);
		if (!data)
			return false;
		bool loaded = png_decode(data, size, &width, &height, &pixels) != 0;
		free(data);
		return loaded;
	}

	uint32_t sample(float u, float v) const
	{
		if (!pixels || !width || !height)
			return 0xFFFF00FF;
		if (u < 0) u = 0;
		if (u > 1) u = 1;
		if (v < 0) v = 0;
		if (v > 1) v = 1;
		uint32_t x = (uint32_t)(u * (width - 1));
		uint32_t y = (uint32_t)((1.0f - v) * (height - 1));
		return pixels[(size_t)y * width + x];
	}
};