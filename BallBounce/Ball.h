#pragma once

#include <vector>
#include "BoundingBox.h"

class Ball {

	public:

		int ID; // ID of the current ball
		
		BoundingBox boundingBox; // Associated Bounding Box
		std::vector<Ball> *ballArray;

		float posX, posY, posZ, velX, velY, velZ; // Position and velocity of the ball

		float radius, mass; // Radius and Mass of the ball

		Ball();
		Ball(int ID, float posX, float posY, float posZ, float velX, float velY, float velZ, float radius, float mass, BoundingBox &boundingBox);

		void moveBall(float timestep, std::vector<std::vector<float>> forceArray);

		void setParams(int ID, float posX, float posY, float posZ, float velX, float velY, float velZ, float radius, float mass, BoundingBox& boundingBox, std::vector<Ball> *ballArray);

		//bool checkCollisions();
};
