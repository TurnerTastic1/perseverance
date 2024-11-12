#include <cmath>
#include "scene.hpp"
#include "util.hpp"

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

Scene::Scene(double dim, int res, int fov, double asp) : dim(dim), res(res), fov(fov), asp(asp), th(0), ph(0), showAxes(true), viewMode(0), moveSpeed(0.7), rotSpeed(0.2), light(false), spin(true) {}

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
// Tree tree = Tree();
// Speeder speeder = Speeder(shiny);

// Variables
double speederZPos = 1;
double speederXPos = 4;

// Textures
int mode = 0; // Texture mode

// Camera parameters
double eyeX = 0, eyeY = 2, eyeZ = 0.0;        // Initial position of the camera
double centerX = 0, centerY = 2, centerZ = 0; // Point the camera is looking at
double upX = 0.0, upY = 1.0, upZ = 0.0;       // Up vector

// Player orientation
double angle = 0.0; // Angle in radians

void Scene::loadTextures()
{
}

void Scene::idle()
{
  // Enviroment logic
  if (light && spin)
  {
    //  Elapsed time in seconds
    double t = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
    zh = fmod(90 * t, 360.0);
  }

  // // Object scene specific logic
  // if (!showcaseSpeeder)
  // {

  //   double t = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
  //   speederZPos = fmod(25 * t, 360);

  //   if (speederXPos <= -4)
  //     speederXPos = 4;
  //   else
  //     speederXPos = speederXPos - 0.01;
  // }

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
  float Emission[] = {0.0, 0.0, 0.01 * emission, 1.0};
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

void initLighting()
{
  // distance = showcaseSpeeder ? distance : distance + (distance * 0.5);
  double ballRadius = 3.0;
  //  Translate intensity to color vectors
  float Ambient[] = {0.01 * ambient, 0.01 * ambient, 0.01 * ambient, 1.0};
  float Diffuse[] = {0.01 * diffuse, 0.01 * diffuse, 0.01 * diffuse, 1.0};
  float Specular[] = {0.01 * specular, 0.01 * specular, 0.01 * specular, 1.0};
  //  Light position
  float Position[] = {distance * Cos(zh), ylight, distance * Sin(zh), 1.0};
  // float pos2[] = {distance * Cos(zh), distance * Sin(zh), 0, 1.0};
  //   Draw light position as ball (still no lighting here)
  glColor3f(1, 1, 1);
  ball(Position[0], Position[1], Position[2], ballRadius);
  // ball(pos2[0], pos2[1], pos2[2], ballRadius);
  //   OpenGL should normalize normal vectors
  glEnable(GL_NORMALIZE);
  //  Enable lighting
  glEnable(GL_LIGHTING);
  //  Location of viewer for specular calculations
  glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, local);
  //  glColor sets ambient and diffuse color materials
  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
  glEnable(GL_COLOR_MATERIAL);
  //  Enable light 0
  glEnable(GL_LIGHT0);
  //  Set ambient, diffuse, specular components and position of light 0
  glLightfv(GL_LIGHT0, GL_AMBIENT, Ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, Diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, Specular);
  glLightfv(GL_LIGHT0, GL_POSITION, Position);
}

void Scene::draw()
{
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
    initLighting();
  }
  else
  {
    glDisable(GL_LIGHTING);
  }

  // Draw enviroment
  // drawEnviroment();

  // No textures from here on
  glDisable(GL_TEXTURE_2D);

  // No lighting from here on
  glDisable(GL_LIGHTING);

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
  // glBindTexture(GL_TEXTURE_2D, pathTexture);
  glColor3f(1, 1, 1);

  // Draw ground
  glBegin(GL_QUADS);

  // Draw the path
  // glColor3f(0.4, 0.27, 0.2);
  glNormal3f(0, 1, 0);
  glTexCoord2f(0, 0);
  glVertex3d(-dim, -0.01, -4);
  glTexCoord2f(1, 0);
  glVertex3d(-dim, -0.01, 4);
  glTexCoord2f(1, 1);
  glVertex3d(dim, -0.01, 4);
  glTexCoord2f(0, 1);
  glVertex3d(dim, -0.01, -4);

  glEnd();

  // glBindTexture(GL_TEXTURE_2D, grassTexture);
  glColor3f(1, 1, 1);

  glBegin(GL_QUADS);

  // Draw the grass
  // glColor3f(0.2, 0.5, 0.4);
  glNormal3f(0, 1, 0);
  glTexCoord2f(0, 0);
  glVertex3d(-dim, -0.01, -dim);
  glTexCoord2f(1, 0);
  glVertex3d(-dim, -0.01, -4);
  glTexCoord2f(1, 1);
  glVertex3d(dim, -0.01, -4);
  glTexCoord2f(0, 1);
  glVertex3d(dim, -0.01, -dim);

  glNormal3f(0, 1, 0);
  glTexCoord2f(0, 0);
  glVertex3d(-dim, -0.01, dim);
  glTexCoord2f(1, 0);
  glVertex3d(-dim, -0.01, 4);
  glTexCoord2f(1, 1);
  glVertex3d(dim, -0.01, 4);
  glTexCoord2f(0, 1);
  glVertex3d(dim, -0.01, dim);

  glEnd();
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
  // else if (ch == 'l' || ch == 'L')
  //   toggleLight();
  // else if (ch == 'k' || ch == 'K')
  //   toggleLightSpin();
  // else if ((light && ch == 'i') || (light && ch == 'I'))
  //   zh = fmod(zh + 5, 360.0);
  // else if ((light && ch == 'o') || (light && ch == 'O'))
  //   zh = fmod(zh - 5, 360.0);
  // else if ((light && ch == 'b') || (light && ch == 'B'))
  //   distance += 1;
  // else if ((light && ch == 'n') || (light && ch == 'N'))
  //   distance -= 1;
  // else if ((light && ch == 'g') || (light && ch == 'G'))
  //   ylight += 0.5;
  // else if ((light && ch == 'h') || (light && ch == 'H'))
  //   ylight -= 0.5;
  // else if (ch == 't')
  //   mode = 1 - mode;

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