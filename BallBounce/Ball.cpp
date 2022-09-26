#include "Ball.h"

bool isCollisions = false;

Ball::Ball() {

}

Ball::Ball(int ID, float posX, float posY, float posZ, float velX, float velY, float velZ, float radius, float mass, BoundingBox &boundingBox) {

	// Assign ID to the ball
	//ID = IDCounter;
	//IDCounter++;

	this->ID = ID;
	this->boundingBox = boundingBox;

	// Initialize the position and velocity of the ball
	this->posX = posX;
	this->posY = posY;
	this->posZ = posZ;
	this->velX = velX;
	this->velY = velY;
	this->velZ = velZ;

	// Initialize the mass and radius of the ball
	this->radius = radius;
	this->mass = mass;

}

void Ball::moveBall(float timestep, std::vector<std::vector<float>> forceArray) {

	// Accelerations along the x,y and z axis
	float accX = 0, accY = 0, accZ = 0;
	float newVelX = 0, newVelY = 0, newVelZ = 0;
	float newPosX = 0, newPosY = 0, newPosZ = 0;

	// Compute net accelerations along the x, y and z axis
	for (int i = 0; i < forceArray.size(); i++) {
		accX = accX + (forceArray[i][0] / mass);
		accY = accY + (forceArray[i][1] / mass);
		accZ = accZ + (forceArray[i][2] / mass);
	}
	Vertex acceleration(accX, accY, accZ);

	// Compute new velocities along the x, y and z positions
	newVelX = velX + accX * timestep;
	newVelY = velY + accY * timestep;
	newVelZ = velZ + accZ * timestep;

	// Compute new positions along the x,y and z positions
	newPosX = posX + ((velX + newVelX) / 2) * timestep;
	newPosY = posY + ((velY + newVelY) / 2) * timestep;
	newPosZ = posZ + ((velZ + newVelZ) / 2) * timestep;


	// Need to check if new positions are outside the bounding box
	for (int faceIndex = 0; faceIndex < boundingBox.faces.size(); faceIndex++) {
		Vertex prevPos(posX, posY, posZ);
		Vertex newPos(newPosX, newPosY, newPosZ);

		float prevDist = boundingBox.faces[faceIndex].getDist(prevPos);
		float newDist = boundingBox.faces[faceIndex].getDist(newPos);

		if (prevDist * newDist < 0) {
			// Collision has occurred
			
			float timeStepFraction = abs(prevDist) / (abs(prevDist) + abs(newDist));

			Vertex prevVelocity(velX, velY, velZ);


			Vertex collisionVelocity = prevVelocity + acceleration * timeStepFraction * timestep;
			Vertex collisionPosition = prevPos + ((prevVelocity + collisionVelocity)/2) * timeStepFraction * timestep;

			Vertex faceNormal = boundingBox.faces[faceIndex].getNormal();

			Vertex normalVelocity;
			if (faceNormal.dot(faceNormal) < 0) {
				normalVelocity = (faceNormal * -1) * collisionVelocity.dot(faceNormal);
			}
			else {
				normalVelocity = faceNormal * collisionVelocity.dot(faceNormal);
			}
			Vertex tangentVelocity = collisionVelocity - normalVelocity;


			float coeffRest = 0.75f; //0.75, 0.9
			float frictionCoefficient = 0.1f;

			Vertex newNormalVelocity = normalVelocity * (-coeffRest);
			Vertex newTangentVelocity = tangentVelocity * (1 - frictionCoefficient); //tangentVelocity - ((tangentVelocity / tangentVelocity.magnitude()) * (std::min(normalVelocity.magnitude() * frictionCoefficient, tangentVelocity.magnitude())));
			Vertex newCollisionVelocity = newNormalVelocity + newTangentVelocity;

			Vertex newVelocity = newCollisionVelocity + acceleration * (1 - timeStepFraction) * timestep;
			Vertex newPosition = collisionPosition + ((newCollisionVelocity + newVelocity) / 2) * (1 - timeStepFraction) * timestep;

			newPosX = newPosition.x;
			newPosY = newPosition.y;
			newPosZ = newPosition.z;

			newVelX = newVelocity.x;
			newVelY = newVelocity.y;
			newVelZ = newVelocity.z;
			
		}
	}

	if (isCollisions) {
		// Check ball - ball Collisions
		for (int ballID = 0; ballID < ballArray->size(); ballID++) {
			if (ballID >= this->ID) {
				continue;
			}

			Vertex currBallCenter(newPosX, newPosY, newPosZ);
			Vertex otherBallCenter(ballArray->at(ballID).posX, ballArray->at(ballID).posY, ballArray->at(ballID).posZ);

			float dist = currBallCenter.dist(otherBallCenter);

			if (dist < (2 * (this->radius))) {
				printf("ball collision has occurred \n");
				// Collision has occurred
				Vertex currBallVelocity(this->velX, this->velY, this->velZ);
				Vertex otherBallVelocity(ballArray->at(ballID).velX, ballArray->at(ballID).velY, ballArray->at(ballID).velZ);

				Vertex relativeVelocity = currBallVelocity - otherBallVelocity;

				Vertex normalVector = currBallCenter - otherBallCenter;
				normalVector = normalVector / normalVector.magnitude();

				Vertex normalVelocity = normalVector * relativeVelocity.dot(normalVector);

				currBallVelocity = currBallVelocity - normalVelocity;
				otherBallVelocity = otherBallVelocity + normalVelocity;

				newVelX = currBallVelocity.x;
				newVelY = currBallVelocity.y;
				newVelZ = currBallVelocity.z;

				//newPosX = posX;
				//newPosY = posY;
				//newPosZ = posZ;

				ballArray->at(ballID).velX = otherBallVelocity.x;
				ballArray->at(ballID).velY = otherBallVelocity.y;
				ballArray->at(ballID).velZ = otherBallVelocity.z;

				//ballArray->at(ballID).posX = otherBallVelocity.x;
				//ballArray->at(ballID).posY = otherBallVelocity.y;
				//ballArray->at(ballID).posZ = otherBallVelocity.z;

			}
		}
	}

	// Replace the new positions and velocities
	posX = newPosX;
	posY = newPosY;
	posZ = newPosZ;

	velX = newVelX;
	velY = newVelY;
	velZ = newVelZ;

}

void Ball::setParams(int ID, float posX, float posY, float posZ, float velX, float velY, float velZ, float radius, float mass, BoundingBox& boundingBox, std::vector<Ball> *ballArray) {

	this->ID = ID;
	this->boundingBox = boundingBox;

	// Initialize the position and velocity of the ball
	this->posX = posX;
	this->posY = posY;
	this->posZ = posZ;
	this->velX = velX;
	this->velY = velY;
	this->velZ = velZ;

	// Initialize the mass and radius of the ball
	this->radius = radius;
	this->mass = mass;

	this -> ballArray = ballArray;

}
