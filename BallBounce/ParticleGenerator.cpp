#include "ParticleGenerator.h"

// Variables deciding the rate at which new particles are added to the simulation
float particleRate = 0.5;
float timeToAddParticles = particleRate;
int newParticlesPerCycle = 150;

float lifespan = 30;
float size = 20;

bool isWind = true;

// The number of threads
#define numThreads 8

float computeArea(Vertex A, Vertex B, Vertex C) {
	float side1 = (A - B).magnitude();
	float side2 = (B - C).magnitude();
	float side3 = (C - A).magnitude();

	float s = (side1 + side2 + side3) / 2;
	float area = sqrt(s * (s - side1) * (s - side2) * (s - side3));

	return area;
}

void moveParticles(float timestep, int lowerIndex, int upperIndex, ParticleGenerator* particleGenerator) {
	
	for (int i = lowerIndex; i < upperIndex; i++) {

		if (particleGenerator->isDeadVector[i] == true) {
			continue;
		}

		float particleAccX = 0.02f * particleGenerator->generateNoise(-1.0f, 1.0f); //0.000034 * generateNoise(-1.0f, 1.0f);
		float particleAccY = 0.01f * particleGenerator->generateNoise(-1.0f, 1.0f); //0.00001 * generateNoise(-1.0f, 1.0f);;
		float particleAccZ = 0.02f * particleGenerator->generateNoise(-1.0f, 1.0f); 

		//printf("Particle accelerations are %f, %f and %f \n", particleAccX, particleAccY, particleAccZ);

		Vertex acceleration(particleAccX, particleAccY, particleAccZ);
		if (isWind) {
			acceleration = acceleration + Vertex(0.1, 0, 0);
		}

		// Compute new velocities along the x, y and z positions
		float newVelX = particleGenerator->velocityVector[i].x + acceleration.x * timestep;
		float newVelY = particleGenerator->velocityVector[i].y + acceleration.y * timestep;
		float newVelZ = particleGenerator->velocityVector[i].z + acceleration.z * timestep;

		// Compute new positions along the x,y and z positions
		float newPosX = particleGenerator->positionVector[i].x + ((particleGenerator->velocityVector[i].x + newVelX) / 2) * timestep;
		float newPosY = particleGenerator->positionVector[i].y + ((particleGenerator->velocityVector[i].y + newVelY) / 2) * timestep;
		float newPosZ = particleGenerator->positionVector[i].z + ((particleGenerator->velocityVector[i].z + newVelZ) / 2) * timestep;

		float posX = particleGenerator->positionVector[i].x;
		float posY = particleGenerator->positionVector[i].y;
		float posZ = particleGenerator->positionVector[i].z;

		float velX = particleGenerator->velocityVector[i].x;
		float velY = particleGenerator->velocityVector[i].y;
		float velZ = particleGenerator->velocityVector[i].z;

		// Need to check if new positions are colliding with planes
		for (int faceIndex = 0; faceIndex < particleGenerator->collisionVector.size(); faceIndex++) {
			Vertex prevPos(posX, posY, posZ);
			Vertex newPos(newPosX, newPosY, newPosZ);

			float prevDist = particleGenerator->collisionVector[faceIndex].getDist(prevPos);
			float newDist = particleGenerator->collisionVector[faceIndex].getDist(newPos);

			if (prevDist * newDist < 0) {
				// Collision has occurred

				float timeStepFraction = abs(prevDist) / (abs(prevDist) + abs(newDist));

				Vertex prevVelocity(velX, velY, velZ);


				Vertex collisionVelocity = prevVelocity + acceleration * timeStepFraction * timestep;
				Vertex collisionPosition = prevPos + ((prevVelocity + collisionVelocity) / 2) * timeStepFraction * timestep;

				// Need to check for point triangle collisions now
				// First we compute the UV coordinates
				float area1 = computeArea(particleGenerator->collisionVector[faceIndex].faceVertices[0], particleGenerator->collisionVector[faceIndex].faceVertices[1], collisionPosition);
				float area2 = computeArea(particleGenerator->collisionVector[faceIndex].faceVertices[1], particleGenerator->collisionVector[faceIndex].faceVertices[2], collisionPosition);
				float area3 = computeArea(particleGenerator->collisionVector[faceIndex].faceVertices[2], particleGenerator->collisionVector[faceIndex].faceVertices[0], collisionPosition);
				float totalArea = computeArea(particleGenerator->collisionVector[faceIndex].faceVertices[0], particleGenerator->collisionVector[faceIndex].faceVertices[1], particleGenerator->collisionVector[faceIndex].faceVertices[2]);

				float u = area1 / totalArea;
				float v = area2 / totalArea;
				float w = area3 / totalArea;

				if ((u > 0) && (v > 0) && ((u + v) <= 1) && ((u + w) <= 1) && ((w + v) <= 1)) {
					// Point Polygon collision has occurred
					Vertex faceNormal = particleGenerator->collisionVector[faceIndex].getNormal();

					Vertex normalVelocity;
					if (faceNormal.dot(faceNormal) < 0) {
						normalVelocity = (faceNormal * -1) * collisionVelocity.dot(faceNormal);
					}
					else {
						normalVelocity = faceNormal * collisionVelocity.dot(faceNormal);
					}
					Vertex tangentVelocity = collisionVelocity - normalVelocity;

					float coeffRest = 0.1f; //0.75, 0.9
					float frictionCoefficient = 0.0f;

					Vertex newNormalVelocity = normalVelocity * (-coeffRest);
					Vertex newTangentVelocity = tangentVelocity * (1 - frictionCoefficient); //tangentVelocity - ((tangentVelocity / tangentVelocity.magnitude()) * (std::min(normalVelocity.magnitude() * frictionCoefficient, tangentVelocity.magnitude())));
					Vertex newCollisionVelocity = newTangentVelocity + newNormalVelocity;

					Vertex newVelocity = newCollisionVelocity + acceleration * (1 - timeStepFraction) * timestep;
					Vertex newPosition = collisionPosition + ((newCollisionVelocity + newVelocity) / 2) * (1 - timeStepFraction) * timestep;
					//Vertex newPosition = collisionPosition + collisionVelocity * (1 - timeStepFraction) * timestep;


					newPosX = newPosition.x;
					newPosY = newPosition.y;
					newPosZ = newPosition.z;

					newVelX = newVelocity.x;
					newVelY = newVelocity.y;
					newVelZ = newVelocity.z;
				}

				else {
					continue;
				}

			}
		}

		// Replace the new positions and velocities
		particleGenerator->positionVector[i].x = newPosX;
		particleGenerator->positionVector[i].y = newPosY;
		particleGenerator->positionVector[i].z = newPosZ;

		particleGenerator->velocityVector[i].x = newVelX;
		particleGenerator->velocityVector[i].y = newVelY;
		particleGenerator->velocityVector[i].z = newVelZ;

		particleGenerator->ageVector[i] = particleGenerator->ageVector[i] + timestep;
		if (particleGenerator->ageVector[i] > particleGenerator->lifespanVector[i]) {
			particleGenerator->isDeadVector[i] = true;
		}
	}
}

ParticleGenerator::ParticleGenerator() {
	this->numParticles = 0;
	this->totalCapacity = 0;
	srand(static_cast <unsigned> (time(0)));
}

ParticleGenerator::ParticleGenerator(int numParticles, Vertex generatorPosition, Vertex generatorVelocity) {
	this->numParticles = numParticles;
	this->totalCapacity = numParticles * 8;
	this->generatorPosition = generatorPosition;
	this->generatorVelocity = generatorVelocity;
	srand(static_cast <unsigned> (time(0)));
}

void ParticleGenerator::setParams(int numParticles, Vertex generatorPosition, Vertex generatorVelocity) {
	this->numParticles = numParticles;
	this->totalCapacity = numParticles * 8;
	this->generatorPosition = generatorPosition;
	this->generatorVelocity = generatorVelocity;
}

void ParticleGenerator::setCollisionVector(std::vector<TriangularFace>& collisionVector) {
	this->collisionVector = collisionVector;
}


void ParticleGenerator::generateParticles(int generatorType) {
	// Point source
	if (generatorType == 0) {
		for (int particleIndex = 0; particleIndex < totalCapacity; particleIndex++) {
			if (particleIndex < numParticles) {
				positionVector.push_back(generateNoiseVertex(-1.0f, 1.0f) + generatorPosition);
				velocityVector.push_back(Vertex(0, 1.0f, 0) + generatorVelocity);
				ageVector.push_back(0);
				lifespanVector.push_back(lifespan);
				sizeVector.push_back(size);
				isDeadVector.push_back(false);
				rotationVector.push_back(0.05 * generateNoise(-180, 180));
				//rotationVector.push_back(180);
			}
			else {
				positionVector.push_back(generateNoiseVertex(-1.0f, 1.0f) + generatorPosition);
				velocityVector.push_back(Vertex(0, 1.0f, 0) + generatorVelocity);
				ageVector.push_back(20);
				lifespanVector.push_back(lifespan);
				sizeVector.push_back(size);
				isDeadVector.push_back(true);
				rotationVector.push_back(0.05 * generateNoise(-180, 180));
				//rotationVector.push_back(180);


				deadParticles.push(particleIndex);
			}
		}
	}
}

void ParticleGenerator::moveParticlesWithThreads(float timestep) {

	generatorPosition = generatorPosition + (generatorVelocity * timestep);
	std::thread threadHandleVector[numThreads];

	for (int i = 0; i < numThreads; i++) {
		int chunkSize = totalCapacity / numThreads;
		int lower = i * chunkSize;
		int upper = (i != (numThreads - 1)) ? ((i + 1) * chunkSize) : totalCapacity;

		threadHandleVector[i] = std::thread(moveParticles, timestep, lower, upper, this);
	}
	
	for (int i = 0; i < numThreads; i++) {
		threadHandleVector[i].join();
	}

	// Add new points to replace the dead ones
	timeToAddParticles = timeToAddParticles - timestep;

	if (timeToAddParticles < 0) {
		timeToAddParticles = particleRate;
		
		for (int i = 0; i < newParticlesPerCycle; i++) {
			int index = this->deadParticles.front();
			this->deadParticles.pop();

			this->positionVector[index] = this->generateNoiseVertex(-1.0f, 1.0f) + this->generatorPosition;
			this->velocityVector[index] = this->generatorVelocity + Vertex(0, 1.0f, 0);
			this->ageVector[index] = 0;
			this->lifespanVector[index] = lifespan;
			this->sizeVector[index] = size;
			this->isDeadVector[index] = false;
		}
	}

	//printf("Thread waiting completed \n");

}


float ParticleGenerator::generateNoise(float lowerLimit, float upperLimit) {
	float num = lowerLimit + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (upperLimit - lowerLimit)));
	return num;
}

Vertex ParticleGenerator::generateNoiseVertex(float lowerLimit, float upperLimit) {
	float noise1 = generateNoise(lowerLimit, upperLimit);
	float noise2 = generateNoise(lowerLimit, upperLimit);
	float noise3 = generateNoise(lowerLimit, upperLimit);

	return Vertex(noise1, noise2, noise3);

}

float ParticleGenerator::linearInterpolate(float lower, float upper, float t) {
	return lower + t * (upper - lower);
}