#pragma once

class Vertex {
public:
	float x, y, z; // Coordinates of the vertex

	Vertex() {
		this->x = 0;
		this->y = 0;
		this->z = 0;
	}

	Vertex(float x, float y, float z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}

	Vertex operator+(const Vertex& other) {
		Vertex vertex;
		vertex.x = this->x + other.x;
		vertex.y = this->y + other.y;
		vertex.z = this->z + other.z;
		return vertex;
	}

	Vertex operator-(const Vertex& other) {
		Vertex vertex;
		vertex.x = this->x - other.x;
		vertex.y = this->y - other.y;
		vertex.z = this->z - other.z;
		return vertex;
	}

	Vertex operator*(float other) {
		Vertex vertex;
		vertex.x = this->x * other;
		vertex.y = this->y * other;
		vertex.z = this->z * other;
		return vertex;
	}

	Vertex operator/(float other) {
		Vertex vertex;
		vertex.x = this->x / other;
		vertex.y = this->y / other;
		vertex.z = this->z / other;
		return vertex;
	}

	float magnitude() {
		return sqrt(powf(this->x, 2) + powf(this->y, 2) + powf(this->z, 2));
	}

	float dot(Vertex& other) {
		return this->x * other.x + this->y * other.y + this->z * other.z;
	}

	float dist(Vertex& other) {
		return sqrt(pow(this->x - other.x, 2) + pow(this->x - other.x, 2) + pow(this->x - other.x, 2));
	}

	Vertex cross(const Vertex& other) {
		Vertex vertex;
		vertex.x = this->y * other.z - this->z * other.y;
		vertex.y = -(this->x * other.z - this->z * other.x);
		vertex.z = this->x * other.y - this->y * other.x;

		return vertex;	
	}

};