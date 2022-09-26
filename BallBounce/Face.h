#pragma once

#include <vector>
#include "Vertex.h"

class Face {
public:
	std::vector<Vertex> faceVertices;

	Face(std::vector<Vertex> faceVertices) {
		this->faceVertices = faceVertices;
	}

	Vertex getNormal() {
		Vertex normal = (this->faceVertices[1] - this->faceVertices[0]).cross(this->faceVertices[0] - this->faceVertices[3]);
		normal = normal / normal.magnitude();
		return normal;
	}

	float getDist(Vertex v) {
		Vertex normal = getNormal();
		return (this->faceVertices[0] - v).dot(normal);
	}
};

class TriangularFace {
public:
	std::vector<Vertex> faceVertices;


	TriangularFace() {}

	TriangularFace(std::vector<Vertex> faceVertices) {
		this->faceVertices = faceVertices;
	}

	void setFaceVertices(std::vector<Vertex> faceVertices) {
		this->faceVertices = faceVertices;
	}

	Vertex getNormal() {
		Vertex normal = (this->faceVertices[1] - this->faceVertices[0]).cross(this->faceVertices[0] - this->faceVertices[2]);
		normal = normal / normal.magnitude();
		return normal;
	}

	float getDist(Vertex v) {
		Vertex normal = getNormal();
		return (this->faceVertices[0] - v).dot(normal);
	}
};