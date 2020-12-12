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
			vertices[i] = { point, {0, 0} };
		}
		model = new Model(count, vertices);
	}

	std::array<glm::vec3, 3> getPoints()
	{
		//std::array<glm::vec3, 3> plane_vectors = plane.getPosAndBasis();
		//return { plane_vectors[0], plane_vectors[0] + plane_vectors[1], plane_vectors[0] + plane_vectors[2] };
		return { model->vertices[0].position, model->vertices[1].position, model->vertices[2].position };
	}

	//glm::mat4x4 getRotationToWorld()
	glm::mat4x4 getRotationToLocal()
	{
		glm::mat4x4 res = { {x_local, 0}, {y_local, 0}, {normal, 0}, {0, 0, 0, 1} };
		return glm::transpose(res);
	}

	glm::mat4x4 getLocalToWorld()
	{
		//return glm::translate(center) * getRotationToWorld();
		return glm::translate(center) * glm::transpose(getRotationToLocal());
	}

	glm::mat4x4 getWorldToLocal()
	{
		//return glm::transpose(getRotationToWorld()) * glm::translate(-center);
		return getRotationToLocal() * glm::translate(-center);
	}
};