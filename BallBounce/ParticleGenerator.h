#pragma once

#include <vector>
#include "Vertex.h"
#include <ctime>
#include <queue>
#include <thread>
#include "Face.h"

class ParticleGenerator
{
	public:
		int totalCapacity;
		int numParticles;
		Vertex generatorPosition;
		Vertex generatorVelocity;

		std::vector<Vertex> positionVector; // Contains the positions of all the points
		std::vector<Vertex> velocityVector; // Contains the velocities of all the points
		std::vector<float> ageVector; // Contains the current age of all the particles
		std::vector<float> lifespanVector; // Contains the lifespans of all the particles
		std::vector<float> colorVector; // Contains the color of each particle
		std::vector<float> massVector; // Contains the mass of each particle
		std::vector<float> sizeVector; // Contains the size of each particle
		std::vector<float> rotationVector; // Contains the rotation in degrees of each particle
		std::vector<bool> isDeadVector; // If particle is dead or not

		std::vector<TriangularFace> collisionVector; // Contains the triangles the points can collide with

		std::queue<int> deadParticles; // Keep track of all the dead particles in the scene

		ParticleGenerator();
		ParticleGenerator(int numParticles, Vertex generatorPosition, Vertex generatorVelocity);
		void setParams(int numParticles, Vertex generatorPosition, Vertex generatorVelocity);
		void setCollisionVector(std::vector<TriangularFace>& collisionVector);
		void generateParticles(int generatorType);
		//DWORD WINAPI moveParticles(void* params);
		void moveParticlesWithThreads(float timestep);

		float generateNoise(float lowerLimit, float upperLimit);
		float linearInterpolate(float lower, float upper, float t);
		Vertex generateNoiseVertex(float lowerLimit, float upperLimit);


};



struct thread_data
{
	float timestep;
	int lowerIndex;
	int upperIndex;
	ParticleGenerator *particleGenerator;
};


