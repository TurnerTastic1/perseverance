#include <cmath>
#include "scene.hpp"
#include "util.hpp"
#include "rover.hpp"

#ifdef USEGLEW
#include <GL/glew.h>
#endif
//  OpenGL with prototypes for glext
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
// Tell Xcode IDE to not gripe about OpenGL deprecation
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#else
#include <GL/glut.h>
#endif

// Cosine and Sine in degrees
#define Cos(x) (cos((x) * 3.14159265 / 180))
#define Sin(x) (sin((x) * 3.14159265 / 180))

Scene::Scene(double dim, int res, int fov, double asp) : dim(dim), res(res), fov(fov), asp(asp), th(0), ph(0), showAxes(true), viewMode(0), moveSpeed(5), rotSpeed(0.2), light(true), spin(true)
{
  textureMode = true;
  isDay = true;
}

/* Globals */
// Light parameters
int light = 1;     // Lighting
int one = 1;       // Unit value
int distance = 10; // Light distance
int inc = 10;      // Ball increment
int smooth = 1;    // Smooth/Flat shading
int local = 0;     // Local Viewer Model
int emission = 0;  // Emission intensity (%)
int ambient = 10;  // Ambient intensity (%)
int diffuse = 50;  // Diffuse intensity (%)
int specular = 0;  // Specular intensity (%)
int shininess = 0; // Shininess (power of two)
float shiny = 1;   // Shininess (value)
int zh = 90;       // Light azimuth
float ylight = 0;  // Elevation of light

// Objects
Rover rover = Rover();

// Variables
double rockX;
double rockZ;

// Textures
int mode = 0; // Texture mode

double groundOffset = 0.0; // Global offset for the ground texture

// Camera parameters
double eyeX = 100, eyeY = 50, eyeZ = 0.0;      // Initial position of the camera
double centerX = 0, centerY = 50, centerZ = 0; // Point the camera is looking at
double upX = 0.0, upY = 1.0, upZ = 0.0;        // Up vector

// Player orientation
double angle = 0.0; // Angle in radians

void Scene::loadTextures()
{
  rover.loadTextures();
  groundTexture = Util::LoadTexBMP("textures/ground_texture.bmp");
  mountainTexture = Util::LoadTexBMP("textures/mountain_texture.bmp");

  resetRock();
}

void Scene::idle()
{
  // Enviroment logic
  if (light && spin)
  {
    //  Elapsed time in seconds
    double t = glutGet(GLUT_ELAPSED_TIME) / 2000.0;
    zh = fmod(90 * t, 360.0);
  }

  // Increment ground offset to simulate movement
  groundOffset += 0.001;

  rockX -= 0.28;

  // If rock goes behind the camera (e.g., rockZ < -50),
  // reset it to appear in front again
  if (rockX < -145)
  {
    resetRock();
  }

  //  Tell GLUT it is necessary to redisplay the scene
  glutPostRedisplay();
}

void Scene::toggleViewMode()
{
  viewMode = (viewMode + 1) % 3;
}

void Scene::toggleLight()
{
  light = !light;
}

void Scene::toggleLightSpin()
{
  spin = !spin;
}

void Scene::resetRock()
{
  // Pick a random Z between 0 and 130
  rockZ = rand() % 130;
  rockX = 145.0;
}

/*
 *  Draw a ball
 *     at (x,y,z)
 *     radius (r)
 */
static void ball(double x, double y, double z, double r)
{
  //  Save transformation
  glPushMatrix();
  //  Offset, scale and rotate
  glTranslated(x, y, z);
  glScaled(r, r, r);
  //  White ball with yellow specular
  float yellow[] = {1.0, 1.0, 0.0, 1.0};
  float Emission[] = {0.0f, 0.0f, 0.01f * emission, 1.0f};
  glColor3f(1, 1, 1);
  glMaterialf(GL_FRONT, GL_SHININESS, shiny);
  glMaterialfv(GL_FRONT, GL_SPECULAR, yellow);
  glMaterialfv(GL_FRONT, GL_EMISSION, Emission);
  //  Bands of latitude
  for (int ph = -90; ph < 90; ph += inc)
  {
    glBegin(GL_QUAD_STRIP);
    for (int th = 0; th <= 360; th += 2 * inc)
    {
      Util::Vertex(th, ph);
      Util::Vertex(th, ph + inc);
    }
    glEnd();
  }
  //  Undo transofrmations
  glPopMatrix();
}

bool doLighting(double dim)
{
  double ballRadius = 20.0;
  float pos2[] = {
      0.0f,
      static_cast<float>(1.2 * dim * Sin(zh)),
      static_cast<float>(1.2 * dim * Cos(zh)),
      1.0f};
  glColor3f(1, 1, 1);
  ball(pos2[0], pos2[1], pos2[2], ballRadius);

  bool lightAboveGround = pos2[1] > 0;

  // Translate intensity to color vectors
  float Ambient[] = {0.0f, 0.0f, 0.0f, 1.0f};
  float Diffuse[] = {0.0f, 0.0f, 0.0f, 1.0f};
  float Specular[] = {0.0f, 0.0f, 0.0f, 1.0f};

  if (lightAboveGround)
  {
    Ambient[0] = Ambient[1] = Ambient[2] = 0.01f * ambient;
    Diffuse[0] = Diffuse[1] = Diffuse[2] = 0.01f * diffuse;
    Specular[0] = Specular[1] = Specular[2] = 0.01f * specular;
  }

  glEnable(GL_NORMALIZE);                                     // OpenGL should normalize normal vectors
  glEnable(GL_LIGHTING);                                      // Enable lighting
  glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, local);          // Location of viewer for specular calculations
  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE); // glColor sets ambient and diffuse color materials
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_LIGHT0); // Enable light 0
  //  Set ambient, diffuse, specular components and position of light 0
  glLightfv(GL_LIGHT0, GL_AMBIENT, Ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, Diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, Specular);
  glLightfv(GL_LIGHT0, GL_POSITION, pos2);

  return pos2[1] > 0;
}

void Scene::draw()
{
  if (isDay)
  {
    glClearColor(0.89, 0.61, 0.33, 1.0);
  }
  else
  {
    glClearColor(0.18, 0.12, 0.2, 1.0);
  }

  // Clear the window and depth buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  glLoadIdentity();

  if (viewMode == 0)
  {
    double cameraX = -2 * dim * Sin(th) * Cos(ph);
    double cameraY = +2 * dim * Sin(ph);
    double cameraZ = +2 * dim * Cos(th) * Cos(ph);
    gluLookAt(cameraX, cameraY, cameraZ, 0, 0, 0, 0, Cos(ph), 0);
  }
  else if (viewMode == 1)
  {
    // Calculate the new center position based on the angle
    centerX = eyeX + sin(angle);
    centerZ = eyeZ - cos(angle);

    // Set the camera view
    gluLookAt(eyeX, eyeY, eyeZ, centerX, centerY, centerZ, upX, upY, upZ);
  }
  else
  {
    glRotatef(ph, 1, 0, 0);
    glRotatef(th, 0, 1, 0);
  }

  // * Lighting
  if (light)
  {
    isDay = doLighting(dim);
  }
  else
  {
    glDisable(GL_LIGHTING);
  }

  glEnable(GL_TEXTURE_2D);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, textureMode ? GL_MODULATE : GL_REPLACE);

  // Draw enviroment
  drawEnviroment();

  // Draw objects
  rover.draw(isDay);

  // No lighting from here on
  glDisable(GL_LIGHTING);

  // No textures from here on
  glDisable(GL_TEXTURE_2D);

  // Draw axes if enabled
  drawAxes();
  // Draw screen info
  drawInfo();

  Util::ErrCheck("display");

  //  Flush and swap buffer
  glFlush();
  glutSwapBuffers();
}

void Scene::drawEnviroment()
{
  glBindTexture(GL_TEXTURE_2D, groundTexture);
  glColor3f(1, 1, 1);

  float groundSize = 150;

  // Draw ground
  glBegin(GL_QUADS);

  // Top face (Positive Y)
  glNormal3f(0, 1, 0);
  glTexCoord2f(0, 0 + groundOffset);
  glVertex3d(-groundSize, -0.01, -groundSize);
  glTexCoord2f(1, 0 + groundOffset);
  glVertex3d(-groundSize, -0.01, groundSize);
  glTexCoord2f(1, 1 + groundOffset);
  glVertex3d(groundSize, -0.01, groundSize);
  glTexCoord2f(0, 1 + groundOffset);
  glVertex3d(groundSize, -0.01, -groundSize);

  // Bottom face (Negative Y)
  glNormal3f(0, -1, 0);
  glTexCoord2f(0, 0);
  glVertex3d(-groundSize, -0.01, -groundSize);
  glTexCoord2f(1, 0);
  glVertex3d(groundSize, -0.01, -groundSize);
  glTexCoord2f(1, 1);
  glVertex3d(groundSize, -0.01, groundSize);
  glTexCoord2f(0, 1);
  glVertex3d(-groundSize, -0.01, groundSize);

  // Front face (Positive Z)
  glNormal3f(0, 0, 1);
  glTexCoord2f(0, 0);
  glVertex3d(-groundSize, -0.01, groundSize);
  glTexCoord2f(1, 0);
  glVertex3d(-groundSize, 0.01, groundSize);
  glTexCoord2f(1, 1);
  glVertex3d(groundSize, 0.01, groundSize);
  glTexCoord2f(0, 1);
  glVertex3d(groundSize, -0.01, groundSize);

  // Back face (Negative Z)
  glNormal3f(0, 0, -1);
  glTexCoord2f(0, 0);
  glVertex3d(-groundSize, -0.01, -groundSize);
  glTexCoord2f(1, 0);
  glVertex3d(-groundSize, 0.01, -groundSize);
  glTexCoord2f(1, 1);
  glVertex3d(groundSize, 0.01, -groundSize);
  glTexCoord2f(0, 1);
  glVertex3d(groundSize, -0.01, -groundSize);

  // Left face (Negative X)
  glNormal3f(-1, 0, 0);
  glTexCoord2f(0, 0);
  glVertex3d(-groundSize, -0.01, -groundSize);
  glTexCoord2f(1, 0);
  glVertex3d(-groundSize, 0.01, -groundSize);
  glTexCoord2f(1, 1);
  glVertex3d(-groundSize, 0.01, groundSize);
  glTexCoord2f(0, 1);
  glVertex3d(-groundSize, -0.01, groundSize);

  // Right face (Positive X)
  glNormal3f(1, 0, 0);
  glTexCoord2f(0, 0);
  glVertex3d(groundSize, -0.01, -groundSize);
  glTexCoord2f(1, 0);
  glVertex3d(groundSize, 0.01, -groundSize);
  glTexCoord2f(1, 1);
  glVertex3d(groundSize, 0.01, groundSize);
  glTexCoord2f(0, 1);
  glVertex3d(groundSize, -0.01, groundSize);

  glEnd();

  glBindTexture(GL_TEXTURE_2D, mountainTexture);
  glColor3f(1, 1, 1);

  glBegin(GL_TRIANGLE_STRIP);

  // The idea: Place a series of peaks and valleys to create a mountainous silhouette
  int mountainDistance = -100;
  glNormal3f(0, 1, 0);
  glTexCoord2f(0, 0);
  glVertex3f(-150, -0.01f, mountainDistance);
  glTexCoord2f(1, 0);
  glVertex3f(-120, 40.0f, mountainDistance - 40);
  glTexCoord2f(1, 1);
  glVertex3f(-100, -0.01f, mountainDistance - 10);

  glNormal3f(0, 1, 0);
  glTexCoord2f(0, 0);
  glVertex3f(-50, 60.0f, mountainDistance - 30);
  glTexCoord2f(1, 0);
  glVertex3f(0, -0.01f, mountainDistance + 30);
  glTexCoord2f(1, 1);
  glVertex3f(50, 75.0f, mountainDistance - 50);

  glNormal3f(0, 1, 0);
  glTexCoord2f(0, 0);
  glVertex3f(100, -0.01f, mountainDistance + 10);
  glTexCoord2f(1, 0);
  glVertex3f(150, 60.0f, mountainDistance - 40);
  glTexCoord2f(1, 1);
  glVertex3f(150, -0.01f, mountainDistance - 40);

  glEnd();

  glBegin(GL_TRIANGLE_STRIP);

  glNormal3f(0, -1, 0);
  glTexCoord2f(0, 0);
  glVertex3f(-150, -0.01f, mountainDistance - 0.1);
  glTexCoord2f(1, 0);
  glVertex3f(-120, 40.0f, mountainDistance - 40 - 0.1);
  glTexCoord2f(1, 1);
  glVertex3f(-100, -0.01f, mountainDistance - 10 - 0.1);

  glNormal3f(0, -1, 0);
  glTexCoord2f(0, 0);
  glVertex3f(-50, 60.0f, mountainDistance - 30 - 0.1);
  glTexCoord2f(1, 0);
  glVertex3f(0, -0.01f, mountainDistance + 30 - 0.1);
  glTexCoord2f(1, 1);
  glVertex3f(50, 75.0f, mountainDistance - 50 - 0.1);

  glNormal3f(0, -1, 0);
  glTexCoord2f(0, 0);
  glVertex3f(100, -0.01f, mountainDistance + 10 - 0.1);
  glTexCoord2f(1, 0);
  glVertex3f(150, 60.0f, mountainDistance - 40 - 0.1);
  glTexCoord2f(1, 1);
  glVertex3f(150, -0.01f, mountainDistance - 40 - 0.1);

  glEnd();

  // Random rock
  glDisable(GL_TEXTURE_2D);
  glColor3f(0.4f, 0.4f, 0.4f); // Gray rock color

  glPushMatrix();
  double rockY = 5; // Slightly above ground
  glTranslated(rockX, rockY, rockZ);

  // Draw a small cube (or use Util::ball)
  double rockSize = 8.0;
  glBegin(GL_QUADS);
  // Top
  glNormal3f(0, 1, 0);
  glVertex3f(-rockSize, 0, -rockSize);
  glVertex3f(rockSize, 0, -rockSize);
  glVertex3f(rockSize, 0, rockSize);
  glVertex3f(-rockSize, 0, rockSize);
  // Sides...
  // Front
  glNormal3f(0, 0, 1);
  glVertex3f(-rockSize, 0, rockSize);
  glVertex3f(rockSize, 0, rockSize);
  glVertex3f(rockSize, -rockSize, rockSize);
  glVertex3f(-rockSize, -rockSize, rockSize);
  // Back
  glNormal3f(0, 0, -1);
  glVertex3f(rockSize, 0, -rockSize);
  glVertex3f(-rockSize, 0, -rockSize);
  glVertex3f(-rockSize, -rockSize, -rockSize);
  glVertex3f(rockSize, -rockSize, -rockSize);
  // Left
  glNormal3f(-1, 0, 0);
  glVertex3f(-rockSize, 0, -rockSize);
  glVertex3f(-rockSize, 0, rockSize);
  glVertex3f(-rockSize, -rockSize, rockSize);
  glVertex3f(-rockSize, -rockSize, -rockSize);
  // Right
  glNormal3f(1, 0, 0);
  glVertex3f(rockSize, 0, rockSize);
  glVertex3f(rockSize, 0, -rockSize);
  glVertex3f(rockSize, -rockSize, -rockSize);
  glVertex3f(rockSize, -rockSize, rockSize);
  // Bottom
  glNormal3f(0, -1, 0);
  glVertex3f(-rockSize, -rockSize, -rockSize);
  glVertex3f(rockSize, -rockSize, -rockSize);
  glVertex3f(rockSize, -rockSize, rockSize);
  glVertex3f(-rockSize, -rockSize, rockSize);
  glEnd();

  glPopMatrix();

  // Re-enable textures if needed
  glEnable(GL_TEXTURE_2D);
}

void Scene::drawAxes()
{
  if (!showAxes)
    return;
  const double len = dim * 0.7; // Length of axes

  //  White
  glColor3f(1, 1, 1);

  glBegin(GL_LINES);
  glVertex3d(0.0, 0.0, 0.0);
  glVertex3d(len, 0.0, 0.0);
  glVertex3d(0.0, 0.0, 0.0);
  glVertex3d(0.0, len, 0.0);
  glVertex3d(0.0, 0.0, 0.0);
  glVertex3d(0.0, 0.0, len);
  glEnd();

  // Label axes
  glRasterPos3d(len, 0.0, 0.0);
  Util::Print("X");
  glRasterPos3d(0.0, len, 0.0);
  Util::Print("Y");
  glRasterPos3d(0.0, 0.0, len);
  Util::Print("Z");
}

void Scene::drawInfo()
{
  //  White
  glColor3f(1, 1, 1);

  glWindowPos2i(5, 5);
  Util::Print("Angle=%d,%d", th, ph);

  glWindowPos2i(5, 25);
  Util::Print("View Mode (m): %s",
              viewMode == 0 ? "Perspective" : viewMode == 1 ? "First person"
                                                            : "Orthographic");

  glWindowPos2i(5, 45);
  Util::Print("Texture Mode (t): %s", textureMode ? "Modulate" : "Replace");

  glWindowPos2i(5, 65);
  Util::Print("Lighting (l): %s", light ? "On" : "Off");
}

void Scene::toggleAxes()
{
  showAxes = !showAxes;
}

void Scene::resetAngles()
{
  th = 0;
  ph = 0;
}

void Scene::toggleTextureMode()
{
  textureMode = !textureMode;
}

void Scene::adjustAngles(int t, int p)
{
  th = (th + t) % 360;
  ph = (ph + p) % 360;
}

void Scene::key(unsigned char ch, int x, int y)
{
  //  Exit on ESC
  if (ch == 27)
    exit(0);
  //  Reset view angle
  else if (ch == 'r' || ch == 'R')
    resetAngles();
  //  Toggle axes
  else if (ch == 'a' || ch == 'A')
    toggleAxes();
  else if (ch == 'm' || ch == 'M')
    toggleViewMode();
  else if (ch == 't' || ch == 'T')
    toggleTextureMode();
  else if (ch == 'l' || ch == 'L')
    toggleLight();
  else if (ch == 'k' || ch == 'K')
    toggleLightSpin();

  if (viewMode == 1)
  {
    switch (ch)
    {
    case 'w':
      centerY += moveSpeed;
      eyeY += moveSpeed;
      break;
    case 's':
      centerY -= moveSpeed;
      eyeY -= moveSpeed;
      break;
    }
  }

  project();

  //  Tell GLUT it is necessary to redisplay the scene
  glutPostRedisplay();
}

void Scene::special(int key, int x, int y)
{
  if (viewMode == 1)
  {
    switch (key)
    {
    case GLUT_KEY_UP:
      eyeX += moveSpeed * sin(angle);
      eyeZ -= moveSpeed * cos(angle);
      break;
    case GLUT_KEY_DOWN:
      eyeX -= moveSpeed * sin(angle);
      eyeZ += moveSpeed * cos(angle);
      break;
    case GLUT_KEY_LEFT:
      angle -= rotSpeed;
      break;
    case GLUT_KEY_RIGHT:
      angle += rotSpeed;
      break;
    }

    // Calculate the new center position based on the angle
    centerX = eyeX + sin(angle);
    centerZ = eyeZ - cos(angle);
  }
  else
  {
    switch (key)
    {
    case GLUT_KEY_UP:
      adjustAngles(0, 5);
      break;
    case GLUT_KEY_DOWN:
      adjustAngles(0, -5);
      break;
    case GLUT_KEY_LEFT:
      adjustAngles(5, 0);
      break;
    case GLUT_KEY_RIGHT:
      adjustAngles(-5, 0);
      break;
    }
  }

  project();

  //  Tell GLUT it is necessary to redisplay the scene
  glutPostRedisplay();
}

void Scene::reshape(int width, int height)
{
  //  Ratio of the width to the height of the window
  asp = (height > 0) ? (double)width / height : 1;
  //  Set the viewport to the entire window
  glViewport(0, 0, res * width, res * height);
  //  Set projection
  project();
}

void Scene::project()
{
  //  Tell OpenGL we want to manipulate the projection matrix
  glMatrixMode(GL_PROJECTION);
  //  Undo previous transformations
  glLoadIdentity();
  //  Perspective transformation
  if (viewMode == 0 || viewMode == 1)
    gluPerspective(fov, asp, dim / 4, 4 * dim);
  //  Orthogonal projection
  else
    glOrtho(-asp * dim, +asp * dim, -dim, +dim, -dim, +dim);
  //  Switch to manipulating the model matrix
  glMatrixMode(GL_MODELVIEW);
  //  Undo previous transformations
  glLoadIdentity();
}