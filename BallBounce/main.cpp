#include <iostream>

#include <vector>
#include <Windows.h>

#include "simulationVariables.h"
#include "Ball.h"
#include "BoundingBox.h"
#include "ParticleGenerator.h"
#include "stb_image.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/glut.h>

#include <chrono>
#include <thread>

BoundingBox boundingBox; //Bounding Box in the scene

// Time related variables
float totalTime = 0.0f; // Simulation elapsed time
std::chrono::steady_clock::time_point startTime, endTime;
float elapsedTime; // Display elapsed time

// Ball related variables
int numBalls = 1;
std::vector<Ball> ballArray; //Vector holding all the current balls in the scene

// Particle Generator
ParticleGenerator particleGenerator;

// Set of collision object
std::vector<TriangularFace> collisionObjects;

void constructBoundingBox() {

    // Construct a bounding box
    std::vector<Face> faces;

    // Face 1 - Bottom
    Vertex v0(-10, -10, -10);
    Vertex v1(10, -10, -10);
    Vertex v2(10, -10, 10);
    Vertex v3(-10, -10, 10);
    std::vector<Vertex> vertices_f0{ v0, v1, v2, v3 };

    Face f0(vertices_f0);

    faces.push_back(f0);

    // Face 2 - left
    Vertex v4(-10, -10, -10);
    Vertex v5(-10, 10, -10);
    Vertex v6(-10, 10, 10);
    Vertex v7(-10, -10, 10);
    std::vector<Vertex> vertices_f1{ v4, v5, v6, v7 };

    Face f1(vertices_f1);

    faces.push_back(f1);

    // Face 3 - right
    Vertex v8(10, 10, -10);
    Vertex v9(10, -10, -10);
    Vertex v10(10, -10, 10);
    Vertex v11(10, 10, 10);
    std::vector<Vertex> vertices_f2{ v8, v9, v10, v11 };

    Face f2(vertices_f2);

    faces.push_back(f2);

    // Face 4 - top
    Vertex v12(-10, 10, -10);
    Vertex v13(10, 10, -10);
    Vertex v14(10, 10, 10);
    Vertex v15(-10, 10, 10);
    std::vector<Vertex> vertices_f3{ v12, v13, v14, v15 };

    Face f3(vertices_f3);

    faces.push_back(f3);

    // Face 5 - front
    Vertex v16(-10, 10, 10);
    Vertex v17(10, 10, 10);
    Vertex v18(10, -10, 10);
    Vertex v19(-10, -10, 10);
    std::vector<Vertex> vertices_f4{ v16, v17, v18, v19 };

    Face f4(vertices_f4);

    faces.push_back(f4);

    // Face 6 - back
    Vertex v20(-10, 10, -10);
    Vertex v21(-10, -10, -10);
    Vertex v22(10, -10, -10);
    Vertex v23(10, 10, -10);
    std::vector<Vertex> vertices_f5{ v20, v21, v22, v23 };

    Face f5(vertices_f5);

    faces.push_back(f5);

    boundingBox.setFaces(faces);
}

float getNoise() {
    float noise = rand() % 200 + 1;
    noise = noise - 100;
    noise = noise / 10.0f;

    return noise;
}

void constructBall() {

    // Spawn a ball
    // Set its position, velocity, radius and mass
    float posX = 0;
    float posY = 0;
    float posZ = 0;

    float velX = 6;
    float velY = 12;
    float velZ = 5;
        
    float radius = 1.0f;
    float mass = 20;

    if (numBalls == 1) {
        Ball ball;
        ball.setParams(0, posX, posY, posZ, velX, velY, velZ, radius, mass, boundingBox, &ballArray);
        ballArray.push_back(ball);
    }

    else if (numBalls == 2) {
        Ball ball;
        velX = 5;
        velY = 6;
        velZ = 0;

        ball.setParams(0, posX, posY, posZ, velX, velY, velZ, radius, mass, boundingBox, &ballArray);
        ballArray.push_back(ball);

        Ball ball2;
        ball2.setParams(1, posX + 5, posY, posZ, -velX, velY, velZ, radius, mass, boundingBox, &ballArray);
        ballArray.push_back(ball2);
    }

    else {
        Ball ball;
        ball.setParams(0, posX, posY, posZ, velX, velY, velZ, radius, mass, boundingBox, &ballArray);
        ballArray.push_back(ball);

        for (int i = 1; i < numBalls; i++) {
            Ball ball_i;
            ball_i.setParams(i, posX + getNoise(), posY + getNoise(), posZ + getNoise(), velX + getNoise(), velY + getNoise(), velZ + getNoise(), radius, mass, boundingBox, &ballArray);
            ballArray.push_back(ball_i);
        }
    }

}

void constructCollisionObject() {
    std::vector<Vertex> faceVertices;
    Vertex shift(7, 0, 0);
    float height = 2;
    float side = 7;
    Vertex v1(-side, height, -side);
    Vertex v2(-side, height, side);
    Vertex v3(side, height, side);
    faceVertices.push_back(v1 + shift);
    faceVertices.push_back(v2 + shift);
    faceVertices.push_back(v3 + shift);

    std::vector<Vertex> faceVertices2;
    Vertex v4(-side, height, -side);
    Vertex v5(side, height, side);
    Vertex v6(side, height, -side);
    faceVertices2.push_back(v4 + shift);
    faceVertices2.push_back(v5 + shift);
    faceVertices2.push_back(v6 + shift);


    TriangularFace face1(faceVertices);
    TriangularFace face2(faceVertices2);

    collisionObjects.push_back(face1);
    collisionObjects.push_back(face2);

}

void constructParticleGenerator() {
    int numParticles = 1000;
    Vertex position = Vertex(0, -10.0f, 0);
    Vertex velocity = Vertex(0.0f, 0, 0);

    particleGenerator.setParams(numParticles, position, velocity);

    int generatorType = 0;
    particleGenerator.generateParticles(generatorType);

}

void display()
{   

    glClear(GL_COLOR_BUFFER_BIT);

    // Draw smoke
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPushMatrix();
    for (int i = 0; i < particleGenerator.totalCapacity; i++) {
        if (particleGenerator.isDeadVector[i] == true) {
            particleGenerator.deadParticles.push(i);
            continue;
        }

        glPointSize(particleGenerator.sizeVector[i]);
        GLfloat colorPoint[] = { 0.9f, 0.9f, 0.9f, 1.0 - (particleGenerator.ageVector[i] / particleGenerator.lifespanVector[i])};
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, colorPoint);

        //Compute rotation
        float rotationVal = particleGenerator.linearInterpolate(0, 180, particleGenerator.positionVector[i].x / 7) + particleGenerator.rotationVector[i];
        glTranslatef(-particleGenerator.positionVector[i].x, -particleGenerator.positionVector[i].y, -particleGenerator.positionVector[i].z);
        glRotatef(rotationVal, 0, 1, 0);
        
        glBegin(GL_POINTS);
        //glVertex3f(particleGenerator.positionVector[i].x, particleGenerator.positionVector[i].y, particleGenerator.positionVector[i].z);
        glVertex3f(0, 0, 0);

        glEnd();

        //Rotate back
        glRotatef(-rotationVal, 0, 1, 0);
        glTranslatef(particleGenerator.positionVector[i].x, particleGenerator.positionVector[i].y, particleGenerator.positionVector[i].z);

    }
    glPopMatrix();
    glDisable(GL_BLEND);

    // Draw Collision Objects
    glPushMatrix();
    GLfloat colorObject[] = { 1.0f, 0.0f, 0.0f, 1 };
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, colorObject);

    for (int i = 0; i < collisionObjects.size(); i++) {
        glBegin(GL_TRIANGLES);
        glVertex3f(collisionObjects[i].faceVertices[0].x, collisionObjects[i].faceVertices[0].y, collisionObjects[i].faceVertices[0].z);
        glVertex3f(collisionObjects[i].faceVertices[1].x, collisionObjects[i].faceVertices[1].y, collisionObjects[i].faceVertices[1].z);
        glVertex3f(collisionObjects[i].faceVertices[2].x, collisionObjects[i].faceVertices[2].y, collisionObjects[i].faceVertices[2].z);
        glEnd();
    }
    glPopMatrix();


    glutSwapBuffers();
}

void idle() {

    // Running the simulation

    if (totalTime >= (1.0f / simulationFPS)) { // Checks simulation time

        endTime = std::chrono::steady_clock::now();
        elapsedTime = (std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime)).count() / pow(10, 6);

        if (elapsedTime >= (1.0f / displayFPS)) { // Checks display time
            startTime = std::chrono::steady_clock::now();
            totalTime = 0;
            glutPostRedisplay();
        }

        else {
            return;
        }
    }

    particleGenerator.moveParticlesWithThreads(timestep);
    totalTime = totalTime + timestep;
    //printf("moved particles \n");
}

void myInit()
{   
    glewExperimental = GL_TRUE;

    glClearColor(1.0, 1.0, 1.0, 1.0);
    glColor3f(1.0, 0.0, 0.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(75.0f, 1.0f, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);

    glLoadIdentity();
    gluLookAt(0, 0, 25, 0, 0, 0, 0, 1, 0);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);

    // Setup light
    GLfloat lightpos[] = { 0, -10, 5, 1};
    glLightfv(GL_LIGHT0, GL_POSITION, lightpos);

    GLfloat lightColor[] = { 1, 1, 1, 1 };
    glLightfv(GL_LIGHT0, GL_AMBIENT_AND_DIFFUSE, lightColor);

    // Setup the scene
    //constructBoundingBox();
    //constructBall();
    //constructCollisionObject();
    constructParticleGenerator();
    particleGenerator.setCollisionVector(collisionObjects);

    startTime = std::chrono::steady_clock::now();

    // Setup the texture
    glEnable(GL_TEXTURE_2D);

    int width, height, nrChannels;
    unsigned char* data = stbi_load("C:/Users/anshu/Documents/Physically Based Modelling/BallBounce/BallBounce/smoke_texture.png", &width, &height, &nrChannels, 4);
    if (data == NULL) {
        printf("Image not loaded correctly\n");
    }
    printf("Loaded image with a width of %dpx, a height of %dpx and %d channels\n", width, height, nrChannels);

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    //// set the texture wrapping/filtering options (on the currently bound texture object)
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

   // glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
    glEnable(GL_POINT_SPRITE);

}


int main(int argc, char** argv) {
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(700, 700);
    glutCreateWindow("Ball Bounce");

    glewInit();

    glutDisplayFunc(display);
    glutIdleFunc(idle);
    myInit();
    glutMainLoop();

}
