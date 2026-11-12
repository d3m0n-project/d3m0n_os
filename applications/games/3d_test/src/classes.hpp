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

struct Edge
{
	size_t start;
	size_t end;
};

class Object
{
public:
	const Vertex *vertices;
	size_t vertices_count;
	const Edge *edges;
	size_t edges_count;

	Object(const Vertex *vertex_list, size_t vertex_count,
		const Edge *edge_list, size_t edge_count)
		: vertices(vertex_list), vertices_count(vertex_count),
		  edges(edge_list), edges_count(edge_count)
	{
	}
};
