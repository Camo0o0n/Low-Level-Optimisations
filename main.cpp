#include <stdlib.h>
#include <GL/glut.h>
#include <list>
#include <iostream>
#include <cmath>
#include <new>


#include <cstdlib>
#include <ctime>
#include <chrono>

#include "globals.h"
#include "Vec3.h"
#include "ColliderObject.h"
#include "Box.h"
#include "Sphere.h"

#include <iostream>
#include "Object.h"
#include "CustomNewDelete.h"
#include "Tracker.h"
#include <thread>
#include <mutex>
#include <vector>
#include "Octree.h"
#include "MemoryPool.h"
#include "TrackerManager.h"
using namespace std::chrono;

// this is the number of falling physical items. 
#define NUMBER_OF_BOXES 500
#define NUMBER_OF_SPHERES 500

// these is where the camera is, where it is looking and the bounds of the continaing box. You shouldn't need to alter these

#define LOOKAT_X 10
#define LOOKAT_Y 10
#define LOOKAT_Z 50

#define LOOKDIR_X 10
#define LOOKDIR_Y 0
#define LOOKDIR_Z 0

int maxTreeDepth = 2;

Box* delBox;
Sphere* delSphere;

std::list<ColliderObject*> colliders;
std::vector<ColliderObject*> collidersQ1;
std::vector<ColliderObject*> collidersQ2;
std::vector<ColliderObject*> collidersQ3;
std::vector<ColliderObject*> collidersQ4;
bool colliderThread = true;

TrackerManager trackerManager;

Tracker* defaultTracker;
Tracker* boxTracker;
Tracker* sphereTracker;


Vec3 centrePoint = Vec3((maxX + minX) / 2, (maxY - minY) / 2, (maxZ + minZ) / 2);
Octree* octree;

MemoryPool* pool;

void BoxCreation(int boxCount, bool initialise) {
    for (int i = 0; i < boxCount; ++i) {
        Box* box = new(pool, boxTracker)  Box();
        // Assign random x, y, and z positions within specified ranges
        box->position.x = static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 20.0f));
        box->position.y = 10.0f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 1.0f));
        box->position.z = static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 20.0f));

        box->size = { 1.0f, 1.0f, 1.0f };

        // Assign random x-velocity between -1.0f and 1.0f
        float randomXVelocity = -1.0f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 2.0f));
        box->velocity = { randomXVelocity, 0.0f, 0.0f };

        // Assign a random color to the box
        box->colour.x = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
        box->colour.y = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
        box->colour.z = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

        colliders.push_back(box);
        if (initialise == false)
        {
            octree->addObject(box);
        }
    }
}

void SphereCreation(int sphereCount, bool initialise) {
    for (int i = 0; i < sphereCount; ++i) {
        Sphere* sphere = new(sphereTracker) Sphere();
        // Assign random x, y, and z positions within specified ranges
        sphere->position.x = static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 20.0f));
        sphere->position.y = 10.0f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 1.0f));
        sphere->position.z = static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 20.0f));

        sphere->size = { 1.0f, 1.0f, 1.0f };

        // Assign random x-velocity between -1.0f and 1.0f
        float randomXVelocity = -1.0f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 2.0f));
        sphere->velocity = { randomXVelocity, 0.0f, 0.0f };

        // Assign a random color to the box
        sphere->colour.x = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
        sphere->colour.y = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
        sphere->colour.z = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

        colliders.push_back(sphere);
        if (initialise == false)
        {
            octree->addObject(sphere);
        }
    }
}

void initScene(int boxCount, int sphereCount) {
    std::cout << "max octree depth (0 being no subregions): " << std::endl;
    std::cin >> maxTreeDepth;
    std::cout << "How many boxes: " << std::endl;
    std::cin >> boxCount;
    std::cout << "How many spheres: " << std::endl;
    std::cin >> sphereCount;
    pool = new MemoryPool(sizeof(Box) + sizeof(Header) + sizeof(Footer), NUMBER_OF_BOXES);
    defaultTracker = trackerManager.createTracker("defaultTracker");
    boxTracker = trackerManager.createTracker("boxTracker");
    sphereTracker = trackerManager.createTracker("sphereTracker");
    BoxCreation(boxCount, true);
    SphereCreation(sphereCount, true);
    octree = new Octree(new BoundingBox(centrePoint, (maxX - minX) / 2 ), maxTreeDepth);
    std::vector<ColliderObject*> t;
    std::move(std::begin(colliders), std::end(colliders), std::back_inserter(t));
    octree->InitObjects(t);
}

// a ray which is used to tap (by default, remove) a box - see the 'mouse' function for how this is used.
bool rayBoxIntersection(const Vec3& rayOrigin, const Vec3& rayDirection, const ColliderObject* box) {
    float tMin = (box->position.x - box->size.x / 2.0f - rayOrigin.x) / rayDirection.x;
    float tMax = (box->position.x + box->size.x / 2.0f - rayOrigin.x) / rayDirection.x;

    if (tMin > tMax) std::swap(tMin, tMax);

    float tyMin = (box->position.y - box->size.y / 2.0f - rayOrigin.y) / rayDirection.y;
    float tyMax = (box->position.y + box->size.y / 2.0f - rayOrigin.y) / rayDirection.y;

    if (tyMin > tyMax) std::swap(tyMin, tyMax);

    if ((tMin > tyMax) || (tyMin > tMax))
        return false;

    if (tyMin > tMin)
        tMin = tyMin;

    if (tyMax < tMax)
        tMax = tyMax;

    float tzMin = (box->position.z - box->size.z / 2.0f - rayOrigin.z) / rayDirection.z;
    float tzMax = (box->position.z + box->size.z / 2.0f - rayOrigin.z) / rayDirection.z;

    if (tzMin > tzMax) std::swap(tzMin, tzMax);

    if ((tMin > tzMax) || (tzMin > tMax))
        return false;

    return true;
}

// used in the 'mouse' tap function to convert a screen point to a point in the world
Vec3 screenToWorld(int x, int y) {
    GLint viewport[4];
    GLdouble modelview[16];
    GLdouble projection[16];
    GLfloat winX, winY, winZ;
    GLdouble posX, posY, posZ;

    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glGetIntegerv(GL_VIEWPORT, viewport);

    winX = (float)x;
    winY = (float)viewport[3] - (float)y;
    glReadPixels(x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);

    gluUnProject(winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);

    return Vec3((float)posX, (float)posY, (float)posZ);
}

void collisionThread(std::vector<ColliderObject*> collidersQ, const float deltaTime)
{
    std::list<ColliderObject*> l;
    std::move(std::begin(collidersQ), std::end(collidersQ), std::back_inserter(l));
    for (ColliderObject* box : collidersQ) {
        box->update(&l, deltaTime);
    }
}

// update the physics: gravity, collision test, collision resolution
void updatePhysics(const float deltaTime) {
    std::vector<std::thread*> threadVector;

    CollStruct* colliderStruct = new CollStruct();

    octree->getObjectList(colliderStruct);

    for (std::vector<ColliderObject*>* list : colliderStruct->collObjList) {
        std::thread* thread = new std::thread(collisionThread, *list, deltaTime);
        threadVector.push_back(thread);
    }

    for (std::thread* t : threadVector) {
        t->join();
    }

    delete colliderStruct;

    threadVector.clear();

    octree->validateObjects();
}

// draw the sides of the containing area
void drawQuad(const Vec3& v1, const Vec3& v2, const Vec3& v3, const Vec3& v4) {
    
    glBegin(GL_QUADS);
    glVertex3f(v1.x, v1.y, v1.z);
    glVertex3f(v2.x, v2.y, v2.z);
    glVertex3f(v3.x, v3.y, v3.z);
    glVertex3f(v4.x, v4.y, v4.z);
    glEnd();
}

// draw the entire scene
void drawScene() {

    // Draw the side wall
    GLfloat diffuseMaterial[] = {0.2f, 0.2f, 0.2f, 1.0f};
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseMaterial);

    // Draw the left side wall
    glColor3f(0.5f, 0.5f, 0.5f); // Set the wall color
    Vec3 leftSideWallV1(minX, 0.0f, maxZ);
    Vec3 leftSideWallV2(minX, 50.0f, maxZ);
    Vec3 leftSideWallV3(minX, 50.0f, minZ);
    Vec3 leftSideWallV4(minX, 0.0f, minZ);
    drawQuad(leftSideWallV1, leftSideWallV2, leftSideWallV3, leftSideWallV4);

    // Draw the right side wall
    glColor3f(0.5f, 0.5f, 0.5f); // Set the wall color
    Vec3 rightSideWallV1(maxX, 0.0f, maxZ);
    Vec3 rightSideWallV2(maxX, 50.0f, maxZ);
    Vec3 rightSideWallV3(maxX, 50.0f, minZ);
    Vec3 rightSideWallV4(maxX, 0.0f, minZ);
    drawQuad(rightSideWallV1, rightSideWallV2, rightSideWallV3, rightSideWallV4);


    // Draw the back wall
    glColor3f(0.5f, 0.5f, 0.5f); // Set the wall color
    Vec3 backWallV1(minX, 0.0f, minZ);
    Vec3 backWallV2(minX, 50.0f, minZ);
    Vec3 backWallV3(maxX, 50.0f, minZ);
    Vec3 backWallV4(maxX, 0.0f, minZ);
    drawQuad(backWallV1, backWallV2, backWallV3, backWallV4);

    /// do a check for a value and if it is correct else return
    for (ColliderObject* box : colliders) {
        box->draw();
    }
}

// called by GLUT - displays the scene
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    gluLookAt(LOOKAT_X, LOOKAT_Y, LOOKAT_Z, LOOKDIR_X, LOOKDIR_Y, LOOKDIR_Z, 0, 1, 0);

    drawScene();

    glutSwapBuffers();
}

// called by GLUT when the cpu is idle - has a timer function you can use for FPS, and updates the physics
// see https://www.opengl.org/resources/libraries/glut/spec3/node63.html#:~:text=glutIdleFunc
// NOTE this may be capped at 60 fps as we are using glutPostRedisplay(). If you want it to go higher than this, maybe a thread will help here. 
void idle() {

    static auto last = steady_clock::now();
    auto old = last;
    last = steady_clock::now();
    const duration<float> frameTime = last - old;
    float deltaTime = frameTime.count();

    updatePhysics(deltaTime);

    // tell glut to draw - note this will cap this function at 60 fps
    glutPostRedisplay();
}

// called the mouse button is tapped
void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        // Get the camera position and direction
        Vec3 cameraPosition(LOOKAT_X, LOOKAT_Y, LOOKAT_Z); // Replace with your actual camera position
        Vec3 cameraDirection(LOOKDIR_X, LOOKDIR_Y, LOOKDIR_Z); // Replace with your actual camera direction

        // Get the world coordinates of the clicked point
        Vec3 clickedWorldPos = screenToWorld(x, y);

        // Calculate the ray direction from the camera position to the clicked point
        Vec3 rayDirection = clickedWorldPos - cameraPosition;
        rayDirection.normalise();

        // Perform a ray-box intersection test and remove the clicked box
        bool clickedObjectOK = false;
        float minIntersectionDistance = std::numeric_limits<float>::max();

        ColliderObject* clickedObject = nullptr;
        int count = 0;
        for (ColliderObject* object : colliders) {
            count++;

            if (rayBoxIntersection(cameraPosition, rayDirection, object)) {
                // Calculate the distance between the camera and the intersected box
                Vec3 diff = object->position - cameraPosition;
                float distance = diff.length();

                // Update the clicked box index if this box is closer to the camera
                if (distance < minIntersectionDistance) {
                    clickedObjectOK = true;
                    minIntersectionDistance = distance;
                    clickedObject = object;
                    std::cout << count << std::endl;
                }
            }
        }

        // Remove the clicked box if any
        if (clickedObjectOK != false) {
            if (clickedObject->object == "box") {
                octree->removeObject(clickedObject);
                colliders.remove(clickedObject);
                pool->Free((char*)clickedObject - sizeof(Header), sizeof(Header) + sizeof(Box) + sizeof(Footer));
            }
            else if (clickedObject->object == "sphere") {
                octree->removeObject(clickedObject);
                colliders.remove(clickedObject);
                delete clickedObject;
            }
            
        }
    }
}

// called when the keyboard is used
void keyboard(unsigned char key, int x, int y) {
    const float impulseMagnitude = 20.0f; // Upward impulse magnitude
    static int* intPointer = nullptr;;
    if (key == ' ') { // Spacebar key
        for (ColliderObject* object : colliders) {
            object->velocity.y += impulseMagnitude;
        }
    }
    else if (key == 't') { // t
        intPointer = new(defaultTracker) int[10];
        std::cout << "Allocated integer array" << std::endl;
    }
    else if (key == 'u') { // u
        if (defaultTracker->pHeapStart != nullptr) {
            delete[]((char*)defaultTracker->pHeapEnd + sizeof(Header));
        }
        std::cout << "Deallocated integer array" << std::endl;
    }
    else if (key == 'w') { // w, walk the heap
        int boxBytes = boxTracker->WalkTheHeap();
        int sphereBytes = sphereTracker->WalkTheHeap();
        std::cout << "Whilst walking the heap the counted bytes are:\n" << "Box tracker bytes: " << boxBytes << "\nSphere tracker bytes: " << sphereBytes << "\nTotal tracker bytes: " << boxBytes + sphereBytes << std::endl;
    }
    else if (key == 'f') { // f, enforce memory corruption
        if (boxTracker->count > 8) {
            memcpy(((ColliderObject*)((char*)(boxTracker->pHeapEnd->pPrev->pPrev->pPrev->pPrev) + sizeof(Header)))->testCharArray, "this is a test buffer IIIIIIIIIIIII overflow", strlen("this is a test buffer IIIIIIIIIIIII overflow"));
        }
        else { std::cout << "Not enough objects" << std::endl; }
    }    
    else if (key == 'h') { // h, enforce memory corruption
        if (boxTracker->count > 8) {
            memcpy((Header*)(char*)boxTracker->pHeapEnd->pPrev->pPrev->testCharArray, "this is a test buffer overflow IIII", strlen("this is a test buffer overflow IIII"));
        } 
        else { std::cout << "Not enough objects" << std::endl; }
    }
    else if (key == 'a') { // a, add cube
        BoxCreation(50, false);
    }
    else if (key == 'A') { // a, add sphere
        SphereCreation(50, false);
    }
    else if (key == 'r') { // r, remove cube
        for (int i = 0; i < 10; i++)
        {
            if (boxTracker->pHeapStart != nullptr) {
                delBox = (Box*)((char*)boxTracker->pHeapStart + sizeof(Header));
                octree->removeObject(delBox);
                colliders.remove(delBox);
                pool->Free((char*)delBox - sizeof(Header), sizeof(Header) + sizeof(Box) + sizeof(Footer));
            }
        }
    }
    else if (key == 'R') { // r, remove sphere
        for (int i = 0; i < 10; i++)
        {
            if (sphereTracker->pHeapStart != nullptr) {
                delSphere = (Sphere*)((char*)sphereTracker->pHeapStart + sizeof(Header));
                octree->removeObject(delSphere);
                colliders.remove(delSphere);
                delete (char*)delSphere;
            }
        }
    }
    else if (key == 'm') { // m, remove all cubes
        trackerManager.printAlloc();
    }
}

// the main function. 
int main(int argc, char** argv) {

    srand(static_cast<unsigned>(time(0))); // Seed random number generator
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1920, 1080);
    glutCreateWindow("Simple Physics Simulation");

    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 800.0 / 600.0, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);

    initScene(NUMBER_OF_BOXES, NUMBER_OF_SPHERES);
    glutDisplayFunc(display);
    glutIdleFunc(idle);

    // it will stick here until the program ends. 
    glutMainLoop();
    return 0;
}
