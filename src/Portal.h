#pragma once

#include <GL/glew.h>
#include <array>
#include "Model.h"

class Portal
{
public:
	Model* model;
	Plane plane;
	glm::vec3 center, x_local, y_local, normal;
	glm::vec2 min_plane_point, max_plane_point;

	/*Portal(Plane plane, Model* model)
		: plane(plane)
	{
		this->model = model;
	}*/

	Portal(Plane plane, glm::vec3 center, glm::vec3 x, glm::vec3 y, int count, glm::vec2* triangle_shape)
		: plane(plane)
	{
		glm::vec3 zero = plane.project(glm::vec3());
		glm::vec3 x_proj = plane.project(x) - zero;
		glm::vec3 y_proj = plane.project(y) - zero;
		this->center = center;
		normal = glm::normalize(glm::cross(x_proj, y_proj));
		x_local = glm::normalize(x_proj);
		y_local = glm::normalize(glm::cross(normal, x_proj));
		Vertex* vertices = new Vertex[count];
		for (int i = 0; i < count; i++)
		{
			glm::vec3 point = center + x_proj * triangle_shape[i].x + y_proj * triangle_shape[i].y;
			glm::vec2 proj = plane.project2D(point);
			if (i == 0) {
				min_plane_point = proj;
				max_plane_point = proj;
			} else {
				if (proj.x < min_plane_point.x)
					min_plane_point.x = proj.x;
				if (proj.y < min_plane_point.y)
					min_plane_point.y = proj.y;
				if (max_plane_point.x < proj.x)
					max_plane_point.x = proj.x;
				if (max_plane_point.y < proj.y)
					max_plane_point.y = proj.y;
			}

			vertices[i] = { point, {0, 0} };
		}
		model = new Model(count, vertices);
	}

	bool needTeleport(glm::vec3 prev, glm::vec3 current)
	{
		float dist1 = plane.getSignedDistance(prev - center);
		float dist2 = plane.getSignedDistance(current - center);
		if (dist1 * dist2 > 0.0 || dist1 == 0.0 && dist2 == 0.0)
			return false;
		dist1 = abs(dist1);
		dist2 = abs(dist2);
		float dist_sum = dist1 + dist2;
		glm::vec2 proj1 = plane.project2D(prev - center);
		glm::vec2 proj2 = plane.project2D(current - center);
		glm::vec2 proj_intersection = (dist1 / dist_sum) * proj1 + (dist2 / dist_sum) * proj2;
		return min_plane_point.x < proj_intersection.x && proj_intersection.x < max_plane_point.x
			&& min_plane_point.y < proj_intersection.y && proj_intersection.y < max_plane_point.y;
	}

	std::array<glm::vec3, 3> getPoints()
	{
		//std::array<glm::vec3, 3> plane_vectors = plane.getPosAndBasis();
		//return { plane_vectors[0], plane_vectors[0] + plane_vectors[1], plane_vectors[0] + plane_vectors[2] };
		return { model->vertices[0].position, model->vertices[1].position, model->vertices[2].position };
	}

	glm::mat4x4 getRotationToLocal()
	{
		glm::mat4x4 res = { {x_local, 0}, {y_local, 0}, {normal, 0}, {0, 0, 0, 1} };
		return glm::transpose(res);
	}

	glm::mat4x4 getLocalToWorld()
	{
		return glm::translate(center) * glm::transpose(getRotationToLocal());
	}

	glm::mat4x4 getWorldToLocal()
	{
		return getRotationToLocal() * glm::translate(-center);
	}
};