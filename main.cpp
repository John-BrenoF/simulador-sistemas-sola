#include <GL/freeglut.h>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <cstdio>
#include <string>

struct Body {
    float x, y, z;
    float vx, vy, vz;
    float mass;
    float radius;
    float r, g, b;
    bool isActive;
    bool isGaseous;
    float weight;
    std::string name;
    
    Body() : x(0), y(0), z(0), vx(0), vy(0), vz(0), mass(1), radius(0.1), 
             r(1), g(1), b(1), isActive(true), isGaseous(false), weight(0), name("Unknown") {}
};

std::vector<Body> bodies;
float G = 0.0667f;
float camX = 0.0f, camY = 0.0f, camZ = 50.0f;
float camYaw = 0.0f, camPitch = 0.0f;
float targetCamX = 0.0f, targetCamY = 0.0f, targetCamZ = 50.0f;
float targetCamYaw = 0.0f, targetCamPitch = 0.0f;
bool keys[256] = {false};
float camSpeed = 2.0f;
float camSensitivity = 0.005f;
bool showMenu = false;
int selectedObject = -1;

void navigateToObject(int objectIndex);
void drawMenu();
float dt = 0.005f;
float minDt = 0.0001f;
float maxDt = 0.01f;
float energy = 0.0f;
float softening = 0.5f;
float spatialGridSize = 20.0f;
float fov = 45.0f;
float nearPlane = 0.1f;
float farPlane = 1000.0f;

void init() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    GLfloat lightPos[] = {0.0f, 0.0f, 0.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    GLfloat diffuse[] = {1.0f, 1.0f, 0.8f, 1.0f};
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    GLfloat specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
    GLfloat ambient[] = {0.1f, 0.1f, 0.1f, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
    srand(time(NULL));

    Body sun;
    sun.x = 0.0f; sun.y = 0.0f; sun.z = 0.0f;
    sun.vx = 0.0f; sun.vy = 0.0f; sun.vz = 0.0f;
    sun.mass = 1000.0f;
    sun.radius = 1.0f;
    sun.r = 1.0f; sun.g = 1.0f; sun.b = 0.8f;
    sun.isGaseous = true;
    sun.name = "Sol";
    bodies.push_back(sun);

    Body companion;
    companion.x = 1.0f; companion.y = 0.0f; companion.z = 0.0f;
    companion.vx = 0.0f; companion.vy = sqrt(G * sun.mass / 1.0f); companion.vz = 0.0f;
    companion.mass = 0.001f;
    companion.radius = 0.15f;
    companion.r = 0.7f; companion.g = 0.5f; companion.b = 0.3f;
    companion.isGaseous = false;
    companion.name = "Companion";
    bodies.push_back(companion);

    Body mercury;
    mercury.x = 3.9f; mercury.y = 0.0f; mercury.z = 0.0f;
    mercury.vx = 0.0f; mercury.vy = sqrt(G * sun.mass / 3.9f); mercury.vz = 0.0f;
    mercury.mass = 0.00016f;
    mercury.radius = 0.07f;
    mercury.r = 0.6f; mercury.g = 0.4f; mercury.b = 0.3f;
    mercury.isGaseous = false;
    mercury.name = "Mercurio";
    bodies.push_back(mercury);

    Body venus;
    venus.x = 7.2f; venus.y = 0.0f; venus.z = 0.0f;
    venus.vx = 0.0f; venus.vy = sqrt(G * sun.mass / 7.2f); venus.vz = 0.0f;
    venus.mass = 0.0024f;
    venus.radius = 0.17f;
    venus.r = 1.0f; venus.g = 0.8f; venus.b = 0.4f;
    venus.isGaseous = false;
    venus.name = "Venus";
    bodies.push_back(venus);

    Body earth;
    earth.x = 10.0f; earth.y = 0.0f; earth.z = 0.0f;
    earth.vx = 0.0f; earth.vy = sqrt(G * sun.mass / 10.0f); earth.vz = 0.0f;
    earth.mass = 0.003f;
    earth.radius = 0.18f;
    earth.r = 0.0f; earth.g = 0.5f; earth.b = 1.0f;
    earth.isGaseous = false;
    earth.name = "Terra";
    bodies.push_back(earth);

    Body moon;
    moon.x = 10.0f + 0.0257f; moon.y = 0.0f; moon.z = 0.0f;
    moon.vx = 0.0f; moon.vy = earth.vy + sqrt(G * earth.mass / 0.0257f); moon.vz = 0.0f;
    moon.mass = 0.000036f;
    moon.radius = 0.05f;
    moon.r = 0.7f; moon.g = 0.7f; moon.b = 0.7f;
    moon.isGaseous = false;
    bodies.push_back(moon);

    Body mars;
    mars.x = 15.2f; mars.y = 0.0f; mars.z = 0.0f;
    mars.vx = 0.0f; mars.vy = sqrt(G * sun.mass / 15.2f); mars.vz = 0.0f;
    mars.mass = 0.00032f;
    mars.radius = 0.1f;
    mars.r = 1.0f; mars.g = 0.3f; mars.b = 0.0f;
    mars.isGaseous = false;
    mars.name = "Marte";
    bodies.push_back(mars);

    Body ceres;
    ceres.x = 27.7f; ceres.y = 0.0f; ceres.z = 0.0f;
    ceres.vx = 0.0f; ceres.vy = sqrt(G * sun.mass / 27.7f); ceres.vz = 0.0f;
    ceres.mass = 0.0000047f;
    ceres.radius = 0.05f;
    ceres.r = 0.5f; ceres.g = 0.5f; ceres.b = 0.5f;
    ceres.isGaseous = false;
    bodies.push_back(ceres);

    for (int i = 0; i < 200; ++i) {
        Body asteroid;
        float angle = (rand() % 360) * M_PI / 180.0f;
        float radius = 25.0f + (rand() % 70) * 0.1f;
        asteroid.x = radius * cos(angle);
        asteroid.y = radius * sin(angle);
        asteroid.z = (rand() % 10 - 5) * 0.02f;
        asteroid.vx = -sqrt(G * sun.mass / radius) * sin(angle);
        asteroid.vy = sqrt(G * sun.mass / radius) * cos(angle);
        asteroid.vz = (rand() % 10 - 5) * 0.01f;
        asteroid.mass = 0.000001f;
        asteroid.radius = 0.02f;
        asteroid.r = 0.5f; asteroid.g = 0.5f; asteroid.b = 0.5f;
        asteroid.isGaseous = false;
        bodies.push_back(asteroid);
    }

    for (int i = 0; i < 100; ++i) {
        Body meteorite;
        float semiMajorAxis = 5.0f + (rand() % 295) * 1.0f;
        float eccentricity = 0.1f + (rand() % 50) * 0.01f;
        float angle = (rand() % 360) * M_PI / 180.0f;
        float radius = semiMajorAxis * (1 - eccentricity * eccentricity) / (1 + eccentricity * cos(angle));
        meteorite.x = radius * cos(angle);
        meteorite.y = radius * sin(angle);
        meteorite.z = (rand() % 20 - 10) * 0.1f;
        float v = sqrt(G * sun.mass * (2 / radius - 1 / semiMajorAxis));
        meteorite.vx = -v * sin(angle);
        meteorite.vy = v * cos(angle);
        meteorite.vz = (rand() % 10 - 5) * 0.01f;
        meteorite.mass = 0.0000005f;
        meteorite.radius = 0.015f;
        meteorite.r = 0.6f; meteorite.g = 0.6f; meteorite.b = 0.6f;
        meteorite.isGaseous = false;
        bodies.push_back(meteorite);
    }

    Body jupiter;
    jupiter.x = 52.0f; jupiter.y = 0.0f; jupiter.z = 0.0f;
    jupiter.vx = 0.0f; jupiter.vy = sqrt(G * sun.mass / 52.0f); jupiter.vz = 0.0f;
    jupiter.mass = 0.95f;
    jupiter.radius = 0.4f;
    jupiter.r = 0.8f; jupiter.g = 0.6f; jupiter.b = 0.2f;
    jupiter.isGaseous = true;
    jupiter.name = "Jupiter";
    bodies.push_back(jupiter);

    Body saturn;
    saturn.x = 95.8f; saturn.y = 0.0f; saturn.z = 0.0f;
    saturn.vx = 0.0f; saturn.vy = sqrt(G * sun.mass / 95.8f); saturn.vz = 0.0f;
    saturn.mass = 0.28f;
    saturn.radius = 0.35f;
    saturn.r = 0.9f; saturn.g = 0.8f; saturn.b = 0.6f;
    saturn.isGaseous = true;
    saturn.name = "Saturno";
    bodies.push_back(saturn);

    Body uranus;
    uranus.x = 191.8f; uranus.y = 0.0f; uranus.z = 0.0f;
    uranus.vx = 0.0f; uranus.vy = sqrt(G * sun.mass / 191.8f); uranus.vz = 0.0f;
    uranus.mass = 0.043f;
    uranus.radius = 0.3f;
    uranus.r = 0.4f; uranus.g = 0.8f; uranus.b = 0.8f;
    uranus.isGaseous = true;
    uranus.name = "Urano";
    bodies.push_back(uranus);

    Body neptune;
    neptune.x = 300.7f; neptune.y = 0.0f; neptune.z = 0.0f;
    neptune.vx = 0.0f; neptune.vy = sqrt(G * sun.mass / 300.7f); neptune.vz = 0.0f;
    neptune.mass = 0.051f;
    neptune.radius = 0.29f;
    neptune.r = 0.2f; neptune.g = 0.4f; neptune.b = 0.8f;
    neptune.isGaseous = true;
    neptune.name = "Netuno";
    bodies.push_back(neptune);

    Body pluto;
    pluto.x = 395.0f; pluto.y = 0.0f; pluto.z = 0.0f;
    pluto.vx = 0.0f; pluto.vy = sqrt(G * sun.mass / 395.0f); pluto.vz = 0.0f;
    pluto.mass = 0.0000065f;
    pluto.radius = 0.04f;
    pluto.r = 0.8f; pluto.g = 0.7f; pluto.b = 0.7f;
    pluto.isGaseous = false;
    pluto.name = "Plutao";
    bodies.push_back(pluto);

    Body haumea;
    haumea.x = 433.0f; haumea.y = 0.0f; haumea.z = 0.0f;
    haumea.vx = 0.0f; haumea.vy = sqrt(G * sun.mass / 433.0f); haumea.vz = 0.0f;
    haumea.mass = 0.000002f;
    haumea.radius = 0.03f;
    haumea.r = 0.9f; haumea.g = 0.9f; haumea.b = 0.9f;
    haumea.isGaseous = false;
    bodies.push_back(haumea);

    Body makemake;
    makemake.x = 458.0f; makemake.y = 0.0f; makemake.z = 0.0f;
    makemake.vx = 0.0f; makemake.vy = sqrt(G * sun.mass / 458.0f); makemake.vz = 0.0f;
    makemake.mass = 0.0000015f;
    makemake.radius = 0.03f;
    makemake.r = 0.8f; makemake.g = 0.8f; makemake.b = 0.9f;
    makemake.isGaseous = false;
    bodies.push_back(makemake);

    Body eris;
    eris.x = 678.0f; eris.y = 0.0f; eris.z = 0.0f;
    eris.vx = 0.0f; eris.vy = sqrt(G * sun.mass / 678.0f); eris.vz = 0.0f;
    eris.mass = 0.0000084f;
    eris.radius = 0.04f;
    eris.r = 0.7f; eris.g = 0.8f; eris.b = 0.9f;
    eris.isGaseous = false;
    bodies.push_back(eris);

    for (int i = 0; i < 50; ++i) {
        Body dust;
        float radius = (rand() % 600) * 1.0f;
        float angle = (rand() % 360) * M_PI / 180.0f;
        dust.x = radius * cos(angle);
        dust.y = radius * sin(angle);
        dust.z = (rand() % 20 - 10) * 0.1f;
        dust.vx = -sqrt(G * sun.mass / radius) * sin(angle) * 0.9f;
        dust.vy = sqrt(G * sun.mass / radius) * cos(angle) * 0.9f;
        dust.vz = (rand() % 10 - 5) * 0.01f;
        dust.mass = 0.0000001f;
        dust.radius = 0.01f;
        dust.r = 0.8f; dust.g = 0.8f; dust.b = 0.8f;
        dust.isGaseous = false;
        bodies.push_back(dust);
    }
}

bool isInFrustum(const Body& b) {
    float camDirX = cos(camPitch) * sin(camYaw);
    float camDirY = sin(camPitch);
    float camDirZ = -cos(camPitch) * cos(camYaw);
    
    float dx = b.x - camX;
    float dy = b.y - camY;
    float dz = b.z - camZ;
    float distance = sqrt(dx*dx + dy*dy + dz*dz);
    
    if (distance < nearPlane || distance > farPlane) return false;
    
    float dot = dx * camDirX + dy * camDirY + dz * camDirZ;
    if (dot < 0) return false;
    
    float angle = acos(dot / distance) * 180.0f / M_PI;
    return angle < fov / 2.0f;
}

void drawBodies() {
    for (size_t i = 0; i < bodies.size(); ++i) {
        const auto& b = bodies[i];
        if (!b.isActive) continue;
        
        if (i != 0 && i < 17 && !isInFrustum(b)) continue;
        if (i >= 17 && b.mass < 0.0001f && !isInFrustum(b)) continue;
        
        float dx = b.x - camX;
        float dy = b.y - camY;
        float dz = b.z - camZ;
        float distance = sqrt(dx*dx + dy*dy + dz*dz);
        
        int slices = b.mass < 0.0001f ? 8 : 10;
        if (distance < 50.0f) slices = (i == 0) ? 40 : 30;
        else if (distance < 100.0f) slices = 20;
        
        glPushMatrix();
        glTranslatef(b.x, b.y, b.z);
        GLUquadric* quad = gluNewQuadric();
        
        float colorVariation = 0.9f + 0.1f * sin(b.x + b.y + b.z);
        float r = b.r * colorVariation;
        float g = b.g * colorVariation;
        float blue = b.b * colorVariation;
        
        if (i == 0) {
            GLfloat emission[] = {1.0f, 1.0f, 0.8f, 1.0f};
            glMaterialfv(GL_FRONT, GL_EMISSION, emission);
            glColor3f(r, g, blue);
            gluSphere(quad, b.radius, slices, slices);
            GLfloat noEmission[] = {0.0f, 0.0f, 0.0f, 1.0f};
            glMaterialfv(GL_FRONT, GL_EMISSION, noEmission);
        } else {
            if (b.isGaseous) {
                GLfloat specular[] = {0.2f, 0.2f, 0.2f, 1.0f};
                glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
                GLfloat shininess[] = {10.0f};
                glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
            } else {
                GLfloat specular[] = {0.5f, 0.5f, 0.5f, 1.0f};
                glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
                GLfloat shininess[] = {50.0f};
                glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
            }
            glColor3f(r, g, blue);
            gluSphere(quad, b.radius, slices, slices);
        }
        
        gluDeleteQuadric(quad);
        glPopMatrix();
    }
}

void drawText(float x, float y, const char* text) {
    glDisable(GL_LIGHTING);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, 800, 0, 600, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    glRasterPos2f(x, y);
    for (const char* c = text; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }
    
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_LIGHTING);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    float camDirX = cos(camPitch) * sin(camYaw);
    float camDirY = sin(camPitch);
    float camDirZ = -cos(camPitch) * cos(camYaw);
    float upX = 0.0f, upY = 1.0f, upZ = 0.0f;
    gluLookAt(camX, camY, camZ, camX + camDirX, camY + camDirY, camZ + camDirZ, upX, upY, upZ);
    
    drawBodies();
    
    drawMenu();
    
    char info[256];
    int activeBodies = 0;
    for (const auto& b : bodies) {
        if (b.isActive) activeBodies++;
    }
    sprintf(info, "Active Bodies: %d | G: %.4f | Energy: %.2f", 
            activeBodies, G, energy);
    drawText(10, 580, info);
    
    sprintf(info, "Earth Weight: %.2f N", bodies[4].weight);
    drawText(10, 560, info);
    
    if (!showMenu) {
        drawText(10, 540, "Controls: WASD-move, QE-up/down, Mouse-look, R-reset, +/-G, P-pause, M-menu");
    } else {
        drawText(10, 540, "Press M to close menu, 1-9 to navigate to objects");
    }
    
    glutSwapBuffers();
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fov, (float)w / h, nearPlane, farPlane);
    glMatrixMode(GL_MODELVIEW);
}

float calculateEnergy() {
    float totalEnergy = 0.0f;
    size_t n = bodies.size();
    
    for (size_t i = 0; i < n; ++i) {
        if (!bodies[i].isActive) continue;
        
        float vSq = bodies[i].vx * bodies[i].vx + bodies[i].vy * bodies[i].vy + bodies[i].vz * bodies[i].vz;
        totalEnergy += 0.5f * bodies[i].mass * vSq;
        
        for (size_t j = i + 1; j < n; ++j) {
            if (!bodies[j].isActive) continue;
            float dx = bodies[j].x - bodies[i].x;
            float dy = bodies[j].y - bodies[i].y;
            float dz = bodies[j].z - bodies[i].z;
            float dist = sqrt(dx*dx + dy*dy + dz*dz + softening*softening);
            totalEnergy -= G * bodies[i].mass * bodies[j].mass / dist;
        }
    }
    return totalEnergy;
}

void checkCollisions() {
    size_t n = bodies.size();
    for (size_t i = 0; i < n; ++i) {
        if (!bodies[i].isActive) continue;
        for (size_t j = i + 1; j < n; ++j) {
            if (!bodies[j].isActive) continue;
            
            float dx = bodies[j].x - bodies[i].x;
            float dy = bodies[j].y - bodies[i].y;
            float dz = bodies[j].z - bodies[i].z;
            float dist = sqrt(dx*dx + dy*dy + dz*dz);
            float minDist = bodies[i].radius + bodies[j].radius;
            
            if (dist < minDist && dist > 0.001f) {
                float totalMass = bodies[i].mass + bodies[j].mass;
                float newX = (bodies[i].x * bodies[i].mass + bodies[j].x * bodies[j].mass) / totalMass;
                float newY = (bodies[i].y * bodies[i].mass + bodies[j].y * bodies[j].mass) / totalMass;
                float newZ = (bodies[i].z * bodies[i].mass + bodies[j].z * bodies[j].mass) / totalMass;
                float newVx = (bodies[i].vx * bodies[i].mass + bodies[j].vx * bodies[j].mass) / totalMass;
                float newVy = (bodies[i].vy * bodies[i].mass + bodies[j].vy * bodies[j].mass) / totalMass;
                float newVz = (bodies[i].vz * bodies[i].mass + bodies[j].vz * bodies[j].mass) / totalMass;
                
                bodies[i].x = newX;
                bodies[i].y = newY;
                bodies[i].z = newZ;
                bodies[i].vx = newVx;
                bodies[i].vy = newVy;
                bodies[i].vz = newVz;
                bodies[i].mass = totalMass;
                bodies[i].radius = pow(bodies[i].radius*bodies[i].radius*bodies[i].radius + 
                                     bodies[j].radius*bodies[j].radius*bodies[j].radius, 1.0f/3.0f);
                
                bodies[i].r = (bodies[i].r * bodies[i].mass + bodies[j].r * bodies[j].mass) / totalMass;
                bodies[i].g = (bodies[i].g * bodies[i].mass + bodies[j].g * bodies[j].mass) / totalMass;
                bodies[i].b = (bodies[i].b * bodies[i].mass + bodies[j].b * bodies[j].mass) / totalMass;
                bodies[i].isGaseous = bodies[i].isGaseous || bodies[j].isGaseous;
                
                bodies[j].isActive = false;
            }
        }
    }
}

bool shouldInteract(const Body& a, const Body& b) {
    float dx = b.x - a.x;
    float dy = b.y - a.y;
    float dz = b.z - a.z;
    float distSq = dx*dx + dy*dy + dz*dz;
    return distSq < spatialGridSize * spatialGridSize;
}

float calculateAdaptiveTimestep() {
    float maxAccel = 0.0f;
    size_t n = bodies.size();
    
    for (size_t i = 0; i < n; ++i) {
        if (!bodies[i].isActive) continue;
        float ax = 0.0f, ay = 0.0f, az = 0.0f;
        
        for (size_t j = 0; j < n; ++j) {
            if (i == j || !bodies[j].isActive) continue;
            float dx = bodies[j].x - bodies[i].x;
            float dy = bodies[j].y - bodies[i].y;
            float dz = bodies[j].z - bodies[i].z;
            float distSq = dx*dx + dy*dy + dz*dz + softening*softening;
            float dist = sqrt(distSq);
            float force = G * bodies[j].mass / distSq;
            ax += force * dx / dist;
            ay += force * dy / dist;
            az += force * dz / dist;
        }
        
        float accel = sqrt(ax*ax + ay*ay + az*az);
        if (accel > maxAccel) maxAccel = accel;
    }
    
    if (maxAccel > 0.0f) {
        float adaptiveDt = 0.05f / sqrt(maxAccel);
        return std::max(minDt, std::min(maxDt, adaptiveDt));
    }
    return dt;
}

void updatePhysics() {
    static float lastEnergy = 0.0f;
    static int energyCheckCounter = 0;
    
    float currentDt = calculateAdaptiveTimestep();
    size_t n = bodies.size();
    std::vector<float> ax(n, 0.0f), ay(n, 0.0f), az(n, 0.0f);
    
    for (size_t i = 0; i < n; ++i) {
        if (!bodies[i].isActive) continue;
        bool isVisible = (i < 17) || isInFrustum(bodies[i]);
        if (!isVisible && bodies[i].mass < 0.0001f) continue;
        
        float totalForce = 0.0f;
        for (size_t j = 0; j < n; ++j) {
            if (i == j || !bodies[j].isActive) continue;
            if (!shouldInteract(bodies[i], bodies[j])) continue;
            
            float dx = bodies[j].x - bodies[i].x;
            float dy = bodies[j].y - bodies[i].y;
            float dz = bodies[j].z - bodies[i].z;
            float distSq = dx*dx + dy*dy + dz*dz + softening*softening;
            float dist = sqrt(distSq);
            float force = G * bodies[j].mass / distSq;
            ax[i] += force * dx / dist / bodies[i].mass;
            ay[i] += force * dy / dist / bodies[i].mass;
            az[i] += force * dz / dist / bodies[i].mass;
            totalForce += G * bodies[i].mass * bodies[j].mass / distSq;
        }
        bodies[i].weight = totalForce;
    }
    
    for (size_t i = 0; i < n; ++i) {
        if (!bodies[i].isActive) continue;
        bool isVisible = (i < 17) || isInFrustum(bodies[i]);
        if (!isVisible && bodies[i].mass < 0.0001f) continue;
        
        bodies[i].vx += 0.5f * ax[i] * currentDt;
        bodies[i].vy += 0.5f * ay[i] * currentDt;
        bodies[i].vz += 0.5f * az[i] * currentDt;
    }
    
    for (size_t i = 0; i < n; ++i) {
        if (!bodies[i].isActive) continue;
        bool isVisible = (i < 17) || isInFrustum(bodies[i]);
        if (!isVisible && bodies[i].mass < 0.0001f) continue;
        
        bodies[i].x += bodies[i].vx * currentDt;
        bodies[i].y += bodies[i].vy * currentDt;
        bodies[i].z += bodies[i].vz * currentDt;
    }
    
    std::fill(ax.begin(), ax.end(), 0.0f);
    std::fill(ay.begin(), ay.end(), 0.0f);
    std::fill(az.begin(), az.end(), 0.0f);
    
    for (size_t i = 0; i < n; ++i) {
        if (!bodies[i].isActive) continue;
        bool isVisible = (i < 17) || isInFrustum(bodies[i]);
        if (!isVisible && bodies[i].mass < 0.0001f) continue;
        
        for (size_t j = 0; j < n; ++j) {
            if (i == j || !bodies[j].isActive) continue;
            if (!shouldInteract(bodies[i], bodies[j])) continue;
            
            float dx = bodies[j].x - bodies[i].x;
            float dy = bodies[j].y - bodies[i].y;
            float dz = bodies[j].z - bodies[i].z;
            float distSq = dx*dx + dy*dy + dz*dz + softening*softening;
            float dist = sqrt(distSq);
            float force = G * bodies[j].mass / distSq;
            ax[i] += force * dx / dist / bodies[i].mass;
            ay[i] += force * dy / dist / bodies[i].mass;
            az[i] += force * dz / dist / bodies[i].mass;
        }
    }
    
    for (size_t i = 0; i < n; ++i) {
        if (!bodies[i].isActive) continue;
        bool isVisible = (i < 17) || isInFrustum(bodies[i]);
        if (!isVisible && bodies[i].mass < 0.0001f) continue;
        
        bodies[i].vx += 0.5f * ax[i] * currentDt;
        bodies[i].vy += 0.5f * ay[i] * currentDt;
        bodies[i].vz += 0.5f * az[i] * currentDt;
    }
    
    checkCollisions();
    
    energyCheckCounter++;
    if (energyCheckCounter >= 100) {
        energy = calculateEnergy();
        if (lastEnergy != 0.0f) {
            float energyError = abs(energy - lastEnergy) / abs(lastEnergy);
            if (energyError > 0.05f) {
                float scale = sqrt(abs(lastEnergy / energy));
                for (auto& b : bodies) {
                    if (!b.isActive) continue;
                    b.vx *= scale;
                    b.vy *= scale;
                    b.vz *= scale;
                }
                energy = calculateEnergy();
            }
        }
        lastEnergy = energy;
        energyCheckCounter = 0;
    }
}

void timer(int value) {
    if (dt > 0) updatePhysics();
    
    float lerpFactor = 0.1f;
    camX += (targetCamX - camX) * lerpFactor;
    camY += (targetCamY - camY) * lerpFactor;
    camZ += (targetCamZ - camZ) * lerpFactor;
    camYaw += (targetCamYaw - camYaw) * lerpFactor;
    camPitch += (targetCamPitch - camPitch) * lerpFactor;
    
    if (keys['w']) {
        targetCamX += sin(camYaw) * camSpeed;
        targetCamZ -= cos(camYaw) * camSpeed;
    }
    if (keys['s']) {
        targetCamX -= sin(camYaw) * camSpeed;
        targetCamZ += cos(camYaw) * camSpeed;
    }
    if (keys['a']) {
        targetCamX -= cos(camYaw) * camSpeed;
        targetCamZ -= sin(camYaw) * camSpeed;
    }
    if (keys['d']) {
        targetCamX += cos(camYaw) * camSpeed;
        targetCamZ += sin(camYaw) * camSpeed;
    }
    if (keys['q']) targetCamY += camSpeed;
    if (keys['e']) targetCamY -= camSpeed;
    
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

void keyboard(unsigned char key, int x, int y) {
    keys[key] = true;
    
    switch(key) {
        case 'r':
            bodies.clear();
            init();
            break;
        case '+':
            G *= 1.1f;
            break;
        case '-':
            G *= 0.9f;
            break;
        case 'p':
            dt = (dt > 0) ? 0.0f : 0.005f;
            break;
        case 'm':
            showMenu = !showMenu;
            break;
        case '1': navigateToObject(0); break;
        case '2': navigateToObject(1); break;
        case '3': navigateToObject(2); break;
        case '4': navigateToObject(3); break;
        case '5': navigateToObject(4); break;
        case '6': navigateToObject(5); break;
        case '7': navigateToObject(6); break;
        case '8': navigateToObject(7); break;
        case '9': navigateToObject(8); break;
    }
}

void keyboardUp(unsigned char key, int x, int y) {
    keys[key] = false;
}

void mouse(int button, int state, int x, int y) {}

void navigateToObject(int objectIndex) {
    if (objectIndex < 0 || objectIndex >= bodies.size()) return;
    
    const Body& obj = bodies[objectIndex];
    float distance = 20.0f;
    
    float dx = obj.vx;
    float dy = obj.vy;
    float dz = obj.vz;
    float length = sqrt(dx*dx + dy*dy + dz*dz);
    
    if (length > 0.001f) {
        dx = -dx / length * distance;
        dy = -dy / length * distance;
        dz = -dz / length * distance;
    } else {
        dx = 0; dy = 0; dz = distance;
    }
    
    targetCamX = obj.x + dx;
    targetCamY = obj.y + dy;
    targetCamZ = obj.z + dz;
    
    float lookX = obj.x - targetCamX;
    float lookY = obj.y - targetCamY;
    float lookZ = obj.z - targetCamZ;
    
    targetCamYaw = atan2(lookX, -lookZ);
    targetCamPitch = atan2(lookY, sqrt(lookX*lookX + lookZ*lookZ));
}

void drawMenu() {
    if (!showMenu) return;
    
    glDisable(GL_LIGHTING);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, 800, 0, 600, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    glColor4f(0.0f, 0.0f, 0.0f, 0.7f);
    glBegin(GL_QUADS);
    glVertex2f(10, 10);
    glVertex2f(300, 10);
    glVertex2f(300, 500);
    glVertex2f(10, 500);
    glEnd();
    
    glColor3f(1.0f, 1.0f, 1.0f);
    drawText(20, 480, "=== MENU DE NAVEGACAO ===");
    
    int yPos = 450;
    for (int i = 0; i < bodies.size() && i < 20; ++i) {
        if (!bodies[i].isActive) continue;
        
        char menuText[100];
        sprintf(menuText, "%d. %s", i+1, bodies[i].name.c_str());
        
        if (selectedObject == i) {
            glColor3f(1.0f, 1.0f, 0.0f);
        } else {
            glColor3f(0.8f, 0.8f, 0.8f);
        }
        
        drawText(20, yPos, menuText);
        yPos -= 20;
    }
    
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_LIGHTING);
}

void motion(int x, int y) {
    static int lastX = -1, lastY = -1;
    if (lastX != -1) {
        targetCamYaw += (x - lastX) * camSensitivity;
        targetCamPitch += (y - lastY) * camSensitivity;
        if (targetCamPitch > 1.57f) targetCamPitch = 1.57f;
        if (targetCamPitch < -1.57f) targetCamPitch = -1.57f;
    }
    lastX = x;
    lastY = y;
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Solar System Simulator");
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutTimerFunc(0, timer, 0);
    glutMainLoop();
    return 0;
}