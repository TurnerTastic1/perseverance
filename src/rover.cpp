#include "rover.hpp"
#include "util.hpp"
#ifdef USEGLEW
#include <GL/glew.h>
#endif

// OpenGL with prototypes for glext
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
// Tell Xcode IDE to not gripe about OpenGL deprecation
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#else
#include <GL/glut.h>
#endif

// Default resolution
// For Retina displays compile with -DRES=2
#ifndef RES
#define RES 1
#endif

#include <cmath> // For mathematical operations

Rover::Rover()
{
  size = 25.0;
  bodyPlacementHeight = 15.0;
}

void Rover::loadTextures()
{
  //  Load textures
  bodyTexture = Util::LoadTexBMP("textures/body_texture.bmp");
  supportTexture = Util::LoadTexBMP("textures/support_texture.bmp");
  wheelTexture = Util::LoadTexBMP("textures/wheel_texture.bmp");
  drillTexture = Util::LoadTexBMP("textures/support_texture.bmp");
  drillBitTexture = Util::LoadTexBMP("textures/drill_bit_texture.bmp");
}

void Rover::draw(bool isDay)
{
  buildBody();            // Build the rover's body
  buildSupports();        // Build the rover's supports
  buildWheels();          // Build the rover's wheels
  buildCamera(isDay);     // Build the rover's camera
  buildRearPowerSource(); // Build the rover's rear power source
  buildArmDrill();        // Build the rover's arm drill
}

void Rover::buildBody()
{
  // //  Set specular color to white
  // float white[] = {1, 1, 1, 1};
  // float black[] = {0, 0, 0, 1};
  // glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 1);
  // glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);
  // glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);
  //
  // glPushMatrix();

  glBindTexture(GL_TEXTURE_2D, bodyTexture);
  glColor3f(1, 1, 1); // Set color to white to not affect texture color

  // Drawing the cuboid using quads
  glBegin(GL_QUADS);

  // Front face
  glNormal3f(0, 0, 1);
  glTexCoord2f(0, 0);
  glVertex3f(-0.75f * size, -0.25f * size + bodyPlacementHeight, 0.4f * size);
  glTexCoord2f(1, 0);
  glVertex3f(0.75f * size, -0.25f * size + bodyPlacementHeight, 0.4f * size);
  glTexCoord2f(1, 1);
  glVertex3f(0.75f * size, 0.25f * size + bodyPlacementHeight, 0.4f * size);
  glTexCoord2f(0, 1);
  glVertex3f(-0.75f * size, 0.25f * size + bodyPlacementHeight, 0.4f * size);

  // Back face
  glNormal3f(0, 0, -1);
  glTexCoord2f(0, 0);
  glVertex3f(-0.75f * size, -0.25f * size + bodyPlacementHeight, -0.4f * size);
  glTexCoord2f(1, 0);
  glVertex3f(0.75f * size, -0.25f * size + bodyPlacementHeight, -0.4f * size);
  glTexCoord2f(1, 1);
  glVertex3f(0.75f * size, 0.25f * size + bodyPlacementHeight, -0.4f * size);
  glTexCoord2f(0, 1);
  glVertex3f(-0.75f * size, 0.25f * size + bodyPlacementHeight, -0.4f * size);

  // Left face
  glNormal3f(-1, 0, 0);
  glTexCoord2f(0, 0);
  glVertex3f(-0.75f * size, -0.25f * size + bodyPlacementHeight, -0.4f * size);
  glTexCoord2f(1, 0);
  glVertex3f(-0.75f * size, -0.25f * size + bodyPlacementHeight, 0.4f * size);
  glTexCoord2f(1, 1);
  glVertex3f(-0.75f * size, 0.25f * size + bodyPlacementHeight, 0.4f * size);
  glTexCoord2f(0, 1);
  glVertex3f(-0.75f * size, 0.25f * size + bodyPlacementHeight, -0.4f * size);

  // Right face
  glNormal3f(1, 0, 0);
  glTexCoord2f(0, 0);
  glVertex3f(0.75f * size, -0.25f * size + bodyPlacementHeight, -0.4f * size);
  glTexCoord2f(1, 0);
  glVertex3f(0.75f * size, -0.25f * size + bodyPlacementHeight, 0.4f * size);
  glTexCoord2f(1, 1);
  glVertex3f(0.75f * size, 0.25f * size + bodyPlacementHeight, 0.4f * size);
  glTexCoord2f(0, 1);
  glVertex3f(0.75f * size, 0.25f * size + bodyPlacementHeight, -0.4f * size);

  // Top face
  glNormal3f(0, 1, 0);
  glTexCoord2f(0, 0);
  glVertex3f(-0.75f * size, 0.25f * size + bodyPlacementHeight, -0.4f * size);
  glTexCoord2f(1, 0);
  glVertex3f(0.75f * size, 0.25f * size + bodyPlacementHeight, -0.4f * size);
  glTexCoord2f(1, 1);
  glVertex3f(0.75f * size, 0.25f * size + bodyPlacementHeight, 0.4f * size);
  glTexCoord2f(0, 1);
  glVertex3f(-0.75f * size, 0.25f * size + bodyPlacementHeight, 0.4f * size);

  // Bottom face
  glNormal3f(0, -1, 0);
  glTexCoord2f(0, 0);
  glVertex3f(-0.75f * size, -0.25f * size + bodyPlacementHeight, -0.4f * size);
  glTexCoord2f(1, 0);
  glVertex3f(0.75f * size, -0.25f * size + bodyPlacementHeight, -0.4f * size);
  glTexCoord2f(1, 1);
  glVertex3f(0.75f * size, -0.25f * size + bodyPlacementHeight, 0.4f * size);
  glTexCoord2f(0, 1);
  glVertex3f(-0.75f * size, -0.25f * size + bodyPlacementHeight, 0.4f * size);

  glEnd();
}

void Rover::buildRearPowerSource()
{
  // Set the color for the power source
  glColor3f(0.0f, 0.0f, 1.0f); // Blue color

  // Draw the rear power source using quads
  glBegin(GL_QUADS);

  // Front face
  glVertex3f(-0.75f * size, -0.25f * size + bodyPlacementHeight, 0.4f * size);
  glVertex3f(-0.85f * size, -0.25f * size + bodyPlacementHeight, 0.4f * size);
  glVertex3f(-1.4f * size, 0.25f * size + bodyPlacementHeight, 0.4f * size);
  glVertex3f(-0.75f * size, 0.25f * size + bodyPlacementHeight, 0.4f * size);

  // Back face
  glVertex3f(-0.75f * size, -0.25f * size + bodyPlacementHeight, -0.4f * size);
  glVertex3f(-0.85f * size, -0.25f * size + bodyPlacementHeight, -0.4f * size);
  glVertex3f(-1.4f * size, 0.25f * size + bodyPlacementHeight, -0.4f * size);
  glVertex3f(-0.75f * size, 0.25f * size + bodyPlacementHeight, -0.4f * size);

  // Left face
  glVertex3f(-0.75f * size, -0.25f * size + bodyPlacementHeight, -0.4f * size);
  glVertex3f(-0.75f * size, -0.25f * size + bodyPlacementHeight, 0.4f * size);
  glVertex3f(-0.75f * size, 0.25f * size + bodyPlacementHeight, 0.4f * size);
  glVertex3f(-0.75f * size, 0.25f * size + bodyPlacementHeight, -0.4f * size);

  // Right face
  glVertex3f(-0.85f * size, -0.25f * size + bodyPlacementHeight, -0.4f * size);
  glVertex3f(-0.85f * size, -0.25f * size + bodyPlacementHeight, 0.4f * size);
  glVertex3f(-1.4f * size, 0.25f * size + bodyPlacementHeight, 0.4f * size);
  glVertex3f(-1.4f * size, 0.25f * size + bodyPlacementHeight, -0.4f * size);

  // // Top face
  // glVertex3f(-0.75f * size, 0.25f * size + bodyPlacementHeight, -0.4f * size);
  // glVertex3f(-0.85f * size, 0.25f * size + bodyPlacementHeight, -0.4f * size);
  // glVertex3f(-0.85f * size, 0.25f * size + bodyPlacementHeight, 0.4f * size);
  // glVertex3f(-0.75f * size, 0.25f * size + bodyPlacementHeight, 0.4f * size);

  // Bottom face
  glVertex3f(-0.75f * size, -0.25f * size + bodyPlacementHeight, -0.4f * size);
  glVertex3f(-0.85f * size, -0.25f * size + bodyPlacementHeight, -0.4f * size);
  glVertex3f(-0.85f * size, -0.25f * size + bodyPlacementHeight, 0.4f * size);
  glVertex3f(-0.75f * size, -0.25f * size + bodyPlacementHeight, 0.4f * size);

  glEnd();
}

void Rover::buildWheels()
{

  glBindTexture(GL_TEXTURE_2D, wheelTexture);
  glColor3f(1, 1, 1); // Set color to white to not affect texture color

  // Parameters for the wheel
  double radius = 4.0; // Large radius for the wheel (adjust as needed)
  double height = 5.0; // Thickness of the wheel

  // * Right side wheels

  // Draw the rear right wheel
  glPushMatrix();
  // Position the wheel
  glTranslated(-0.75 * size, bodyPlacementHeight * 0.3, 0.5 * size); // Position the wheel
  drawWheel(radius, height);                                         // Draw the wheel
  glPopMatrix();

  // Draw the mid right wheel
  glPushMatrix();
  // Position the wheel
  glTranslated(0.05 * size, bodyPlacementHeight * 0.3, 0.5 * size); // Position the wheel
  drawWheel(radius, height);                                        // Draw the wheel
  glPopMatrix();

  // Draw the front right wheel
  glPushMatrix();
  // Position the wheel
  glTranslated(0.75 * size, bodyPlacementHeight * 0.3, 0.5 * size); // Position the wheel
  drawWheel(radius, height);                                        // Draw the wheel
  glPopMatrix();

  // * Left side wheels

  // Draw the rear left wheel
  glPushMatrix();
  // Position the wheel
  glTranslated(-0.75 * size, bodyPlacementHeight * 0.3, -0.7 * size); // Position the wheel
  drawWheel(radius, height);                                          // Draw the wheel
  glPopMatrix();

  // Draw the mid left wheel
  glPushMatrix();
  // Position the wheel
  glTranslated(0.05 * size, bodyPlacementHeight * 0.3, -0.7 * size); // Position the wheel
  drawWheel(radius, height);                                         // Draw the wheel
  glPopMatrix();

  // Draw the front left wheel
  glPushMatrix();
  // Position the wheel
  glTranslated(0.75 * size, bodyPlacementHeight * 0.3, -0.7 * size); // Position the wheel
  drawWheel(radius, height);                                         // Draw the wheel
  glPopMatrix();
}

void Rover::drawWheel(double radius, double height)
{
  // Set the wheel's color (optional)
  glColor3f(0.3f, 0.3f, 0.3f); // Dark gray color for the wheel

  // Draw the cylinder for the wheel
  glPushMatrix();

  int segments = 36;                       // Number of segments for smoothness of the wheel surface
  double step = 2.0 * Util::PI / segments; // Incremental angle for each segment

  // Draw the wheel using GL_QUAD_STRIP
  glBegin(GL_QUAD_STRIP);
  for (int i = 0; i <= segments; ++i)
  {
    double angle = i * step;
    double x = radius * cos(angle);
    double y = radius * sin(angle);

    // Outer circle (top and bottom vertices)
    glNormal3d(cos(angle), sin(angle), 0.0); // Normal for lighting
    glTexCoord2f((float)i / segments, 0.0);
    glVertex3d(x, y, 0.0); // Bottom vertex
    glTexCoord2f((float)i / segments, 1.0);
    glVertex3d(x, y, height); // Top vertex
  }
  glEnd();

  // Draw the inner spoke center using GL_QUAD_STRIP
  glColor3f(0.5f, 0.5f, 0.5f);
  double innerRadius = radius / 4; // Radius for the inner circle
  glBegin(GL_QUAD_STRIP);
  for (int i = 0; i <= segments; ++i)
  {
    double angle = i * step;
    double x = innerRadius * cos(angle);
    double y = innerRadius * sin(angle);

    // Inner circle (top and bottom vertices)
    glNormal3d(cos(angle), sin(angle), 0.0); // Normal for lighting
    glTexCoord2f((float)i / segments, 0.0);
    glVertex3d(x, y, height / 1.5); // Bottom vertex
    glTexCoord2f((float)i / segments, 1.0);
    glVertex3d(x, y, height / 2); // Top vertex
  }
  glEnd();

  // Draw 6 evenly spaced spokes connecting the inner circle to the outer circle
  glLineWidth(4.0f); // Increase line width for spokes
  glBegin(GL_LINES);
  int spokeCount = 6;
  for (int i = 0; i < spokeCount; ++i)
  {
    double angle = i * (2.0 * Util::PI / spokeCount); // Calculate the angle for each spoke

    // Outer circle coordinates
    double outerX = radius * cos(angle);
    double outerY = radius * sin(angle);

    // Inner circle coordinates
    double innerX = innerRadius * cos(angle);
    double innerY = innerRadius * sin(angle);

    // Draw line (spoke) from inner circle to outer circle
    glTexCoord2f(0.0, 0.0);
    glVertex3d(innerX, innerY, height / 2); // Inner point
    glTexCoord2f(1.0, 1.0);
    glVertex3d(outerX, outerY, height / 2); // Outer point
  }
  glEnd();
  glLineWidth(1.0f); // Reset line width to default

  glPopMatrix();
}

void Rover::buildCamera(bool isDay)
{
  // * Camera arm
  double cameraArmStart[3] = {
      0.62 * size,
      bodyPlacementHeight + (bodyPlacementHeight * 0.4),
      0.27 * size}; // Start point

  double cameraArmEnd[3] = {
      0.62 * size,
      bodyPlacementHeight * 2.0,
      0.27 * size}; // End point

  drawSupport(0.8, cameraArmStart, cameraArmEnd, -1);

  // * Camera (Rectangular Prism)
  // Set the color for the camera
  // glColor3f(1.0f, 1.0f, 1.0f); // White color

  glBindTexture(GL_TEXTURE_2D, bodyTexture);
  glColor3f(1, 1, 1); // Set color to white to not affect texture color

  // Dimensions for the rectangular camera
  double cubeWidth = 0.17 * size;  // X-axis
  double cubeHeight = 0.17 * size; // Y-axis
  double cubeDepth = 0.25 * size;  // Z-axis (longer side)

  double centerX = cameraArmEnd[0];
  double centerY = cameraArmEnd[1] + (cubeHeight / 2.0); // Position it on top of the arm
  double centerZ = cameraArmEnd[2];

  // Define half-dimensions for easier calculations
  double halfWidth = cubeWidth / 2.0;
  double halfHeight = cubeHeight / 2.0;
  double halfDepth = cubeDepth / 2.0;

  // Drawing the camera using quads
  glBegin(GL_QUADS);

  // Front face (Positive Z)
  glNormal3f(0.0f, 0.0f, 1.0f); // Normal pointing forward
  glTexCoord2f(0, 0);
  glVertex3f(centerX - halfWidth, centerY - halfHeight, centerZ + halfDepth);
  glTexCoord2f(1, 0);
  glVertex3f(centerX + halfWidth, centerY - halfHeight, centerZ + halfDepth);
  glTexCoord2f(1, 1);
  glVertex3f(centerX + halfWidth, centerY + halfHeight, centerZ + halfDepth);
  glTexCoord2f(0, 1);
  glVertex3f(centerX - halfWidth, centerY + halfHeight, centerZ + halfDepth);

  // Back face (Negative Z)
  glNormal3f(0.0f, 0.0f, -1.0f); // Normal pointing backward
  glTexCoord2f(0, 0);
  glVertex3f(centerX - halfWidth, centerY - halfHeight, centerZ - halfDepth);
  glTexCoord2f(1, 0);
  glVertex3f(centerX + halfWidth, centerY - halfHeight, centerZ - halfDepth);
  glTexCoord2f(1, 1);
  glVertex3f(centerX + halfWidth, centerY + halfHeight, centerZ - halfDepth);
  glTexCoord2f(0, 1);
  glVertex3f(centerX - halfWidth, centerY + halfHeight, centerZ - halfDepth);

  // Left face (Negative X)
  glNormal3f(-1.0f, 0.0f, 0.0f); // Normal pointing left
  glTexCoord2f(0, 0);
  glVertex3f(centerX - halfWidth, centerY - halfHeight, centerZ - halfDepth);
  glTexCoord2f(1, 0);
  glVertex3f(centerX - halfWidth, centerY - halfHeight, centerZ + halfDepth);
  glTexCoord2f(1, 1);
  glVertex3f(centerX - halfWidth, centerY + halfHeight, centerZ + halfDepth);
  glTexCoord2f(0, 1);
  glVertex3f(centerX - halfWidth, centerY + halfHeight, centerZ - halfDepth);

  // Right face (Positive X)
  glNormal3f(1.0f, 0.0f, 0.0f); // Normal pointing right
  glTexCoord2f(0, 0);
  glVertex3f(centerX + halfWidth, centerY - halfHeight, centerZ - halfDepth);
  glTexCoord2f(1, 0);
  glVertex3f(centerX + halfWidth, centerY - halfHeight, centerZ + halfDepth);
  glTexCoord2f(1, 1);
  glVertex3f(centerX + halfWidth, centerY + halfHeight, centerZ + halfDepth);
  glTexCoord2f(0, 1);
  glVertex3f(centerX + halfWidth, centerY + halfHeight, centerZ - halfDepth);

  // Top face (Positive Y)
  glNormal3f(0.0f, 1.0f, 0.0f); // Normal pointing up
  glTexCoord2f(0, 0);
  glVertex3f(centerX - halfWidth, centerY + halfHeight, centerZ - halfDepth);
  glTexCoord2f(1, 0);
  glVertex3f(centerX + halfWidth, centerY + halfHeight, centerZ - halfDepth);
  glTexCoord2f(1, 1);
  glVertex3f(centerX + halfWidth, centerY + halfHeight, centerZ + halfDepth);
  glTexCoord2f(0, 1);
  glVertex3f(centerX - halfWidth, centerY + halfHeight, centerZ + halfDepth);

  // Bottom face (Negative Y)
  glNormal3f(0.0f, -1.0f, 0.0f); // Normal pointing down
  glTexCoord2f(0, 0);
  glVertex3f(centerX - halfWidth, centerY - halfHeight, centerZ - halfDepth);
  glTexCoord2f(1, 0);
  glVertex3f(centerX + halfWidth, centerY - halfHeight, centerZ - halfDepth);
  glTexCoord2f(1, 1);
  glVertex3f(centerX + halfWidth, centerY - halfHeight, centerZ + halfDepth);
  glTexCoord2f(0, 1);
  glVertex3f(centerX - halfWidth, centerY - halfHeight, centerZ + halfDepth);

  glEnd();

  // * Camera lens (Sphere)
  // Set the color for the camera lens
  // glColor3f(0.0f, 0.0f, 0.0f); // Black color
  glBindTexture(GL_TEXTURE_2D, wheelTexture);
  glColor3f(1, 1, 1); // Set color to white to not affect texture color

  // Define the lens position and size
  double lensRadius = 0.05 * size; // Adjust as needed
  double lensX = centerX + (halfWidth * 0.7);
  double lensY = centerY;
  double lensZ = centerZ - (halfDepth * 0.5); // Slightly protrude outwards

  // Draw the sphere (lens) using Util::ball
  Util::ball(lensX, lensY, lensZ, lensRadius, 10.0, 50.0, 0.0);

  // **Add Light Source at the Lens When It's Night**
  if (!isDay)
  {
    // Enable GL_LIGHT1
    glEnable(GL_LIGHT1);

    // Set the light's position (at the lens)
    GLfloat lightPos[] = {(GLfloat)lensX, (GLfloat)lensY, (GLfloat)lensZ, 1.0f}; // Positional light
    glLightfv(GL_LIGHT1, GL_POSITION, lightPos);

    // Set ambient, diffuse, and specular components for brighter light
    GLfloat ambient[] = {0.4f, 0.4f, 0.4f, 1.0f};  // Increased ambient
    GLfloat diffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};  // Maxed out diffuse
    GLfloat specular[] = {1.0f, 1.0f, 1.0f, 1.0f}; // Maxed out specular
    glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, specular);

    // Adjust attenuation for a more focused and brighter light
    glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1.0f);   // Remains the same
    glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.05f);    // Reduced linear attenuation
    glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.02f); // Further reduced quadratic attenuation

    // Enhanced spotlight settings for better illumination
    GLfloat spotDirection[] = {1.0f, 0.0f, 0.0f}; // Ensure this points correctly based on camera orientation
    glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spotDirection);
    glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 45.0f);   // Increased cone angle for wider coverage
    glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 20.0f); // Increased concentration for sharper spotlight

    // **Render the Beam of Light (Cone)**
    // Enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Set the cone color with transparency
    glColor4f(1.0f, 1.0f, 1.0f, 0.3f);

    // Disable lighting on the cone to make it emissive
    glDisable(GL_LIGHTING);

    // Set emissive material property
    GLfloat emissive[] = {1.0f, 1.0f, 1.0f, 0.3f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emissive);

    // Beam parameters
    double beamLength = 100.0; // Adjust the length of the beam
    double beamAngle = 15.0;   // Beam angle in degrees (half-angle of the cone)
    int segments = 36;         // Number of segments around the base
    double beamRadius = beamLength * tan(beamAngle * M_PI / 180.0);

    // Save current transformation matrix
    glPushMatrix();

    // Translate to the lens position
    glTranslatef((GLfloat)lensX, (GLfloat)lensY, (GLfloat)lensZ);

    // Align the cone with the beam direction
    // Beam direction (adjust if needed)
    double dirX = -1.0; // Assuming the beam points along the positive X-axis
    double dirY = 0.0;
    double dirZ = 0.0;

    // Calculate rotation axis and angle
    double upX = 0.0, upY = 1.0, upZ = 0.0; // Up vector
    double rotationAxisX = upY * dirZ - upZ * dirY;
    double rotationAxisY = upZ * dirX - upX * dirZ;
    double rotationAxisZ = upX * dirY - upY * dirX;
    double rotationAngle = acos(dirY) * 180.0 / M_PI;

    // Avoid division by zero
    if (rotationAxisX != 0 || rotationAxisY != 0 || rotationAxisZ != 0)
    {
      glRotatef((GLfloat)rotationAngle, (GLfloat)rotationAxisX, (GLfloat)rotationAxisY, (GLfloat)rotationAxisZ);
    }

    // Draw the cone
    glBegin(GL_TRIANGLE_FAN);

    // Apex of the cone at the origin (lens position)
    glVertex3f(0.0f, 0.0f, 0.0f);

    // Base of the cone
    for (int i = 0; i <= segments; ++i)
    {
      double theta = i * 2.0 * M_PI / segments;
      double x = beamRadius * cos(theta);
      double z = beamRadius * sin(theta);
      glVertex3f((GLfloat)x, (GLfloat)-beamLength, (GLfloat)z);
    }

    glEnd();

    // Restore the transformation matrix
    glPopMatrix();

    // Disable blending
    glDisable(GL_BLEND);

    // Re-enable lighting if needed
    glEnable(GL_LIGHTING);

    // Reset emissive material property
    GLfloat no_emissive[] = {0.0f, 0.0f, 0.0f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, no_emissive);
  }
  else
  {
    // Disable GL_LIGHT1 during the day
    glDisable(GL_LIGHT1);
  }
}

void Rover::buildArmDrill()
{
  // * Drill arm
  // Set the color for the drill arm
  glColor3f(1.0f, 1.0f, 1.0f); // White color

  double drillArmStart[3] = {0.75 * size, bodyPlacementHeight, -0.38 * size};
  double drillArmEnd[3] = {1.0 * size, bodyPlacementHeight * 0.95, -0.08 * size};
  drawSupport(0.8, drillArmStart, drillArmEnd, bodyTexture);

  double drillArmStart2[3] = {1.0 * size, bodyPlacementHeight * 0.95, -0.08 * size};
  double drillArmEnd2[3] = {1.3 * size, bodyPlacementHeight * 1.3, 0.4 * size};
  drawSupport(0.8, drillArmStart2, drillArmEnd2, bodyTexture);

  // * Vertical drill machine
  // Set the color for the drill machine
  // glColor3f(0.0f, 0.1f, 0.0f); // Dark green color

  // Vertical support for the drill machine
  double drillMachineStart[3] = {1.3 * size, bodyPlacementHeight * 1.4, 0.4 * size};
  double drillMachineEnd[3] = {1.3 * size, bodyPlacementHeight * 0.9, 0.4 * size};
  drawSupport(2.5, drillMachineStart, drillMachineEnd, drillTexture);

  // * Drill bit
  double drillBitStart[3] = {1.3 * size, bodyPlacementHeight * 1.5, 0.4 * size};
  double drillBitEnd[3] = {1.3 * size, bodyPlacementHeight * 0.8, 0.4 * size};
  drawSupport(0.5, drillBitStart, drillBitEnd, wheelTexture);

  // * Drill bit supports
  double drillBitSupport1Start[3] = {1.25 * size, bodyPlacementHeight * 1.5, 0.4 * size};
  double drillBitSupport1End[3] = {1.25 * size, bodyPlacementHeight * 0.8, 0.4 * size};
  drawSupport(0.5, drillBitSupport1Start, drillBitSupport1End, drillTexture);

  double drillBitSupport2Start[3] = {1.35 * size, bodyPlacementHeight * 1.5, 0.4 * size};
  double drillBitSupport2End[3] = {1.35 * size, bodyPlacementHeight * 0.8, 0.4 * size};
  drawSupport(0.5, drillBitSupport2Start, drillBitSupport2End, drillTexture);
}

void Rover::buildSupports()
{
  // // Dark gun metal color for the supports
  // glColor3f(0.2f, 0.2f, 0.2f);

  // * First support
  double supportOneStart[3] = {-0.75 * size, bodyPlacementHeight * .68, 0.5 * size}; // Start point
  double supportOneEnd[3] = {-0.30 * size, bodyPlacementHeight * .68, 0.5 * size};   // End point
  drawSupport(1.0, supportOneStart, supportOneEnd, -1);

  double supportOneStartA[3] = {-0.30 * size, bodyPlacementHeight * .68, 0.5 * size};
  double supportOneEndA[3] = {0.22 * size, bodyPlacementHeight, 0.5 * size};
  drawSupport(1.0, supportOneStartA, supportOneEndA, -1);

  double supportOneStartB[3] = {0.22 * size, bodyPlacementHeight, 0.5 * size};
  double supportOneEndB[3] = {0.75 * size, bodyPlacementHeight * 0.68, 0.5 * size};
  drawSupport(1.0, supportOneStartB, supportOneEndB, -1);

  double supportOneStartC[3] = {-0.30 * size, bodyPlacementHeight * 0.68, 0.5 * size};
  double supportOneEndC[3] = {0.05 * size, bodyPlacementHeight * 0.4, 0.5 * size};
  drawSupport(0.6, supportOneStartC, supportOneEndC, -1);

  // * Second support
  double supportTwoStart[3] = {-0.75 * size, bodyPlacementHeight * .68, -0.5 * size}; // Start point
  double supportTwoEnd[3] = {-0.30 * size, bodyPlacementHeight * .68, -0.5 * size};   // End point
  drawSupport(1.0, supportTwoStart, supportTwoEnd, -1);

  double supportTwoStartA[3] = {-0.30 * size, bodyPlacementHeight * .68, -0.5 * size};
  double supportTwoEndA[3] = {0.22 * size, bodyPlacementHeight, -0.5 * size};
  drawSupport(1.0, supportTwoStartA, supportTwoEndA, -1);

  double supportTwoStartB[3] = {0.22 * size, bodyPlacementHeight, -0.5 * size};
  double supportTwoEndB[3] = {0.75 * size, bodyPlacementHeight * 0.68, -0.5 * size};
  drawSupport(1.0, supportTwoStartB, supportTwoEndB, -1);

  double supportTwoStartC[3] = {-0.30 * size, bodyPlacementHeight * 0.68, -0.5 * size};
  double supportTwoEndC[3] = {0.05 * size, bodyPlacementHeight * 0.4, -0.5 * size};
  drawSupport(0.6, supportTwoStartC, supportTwoEndC, -1);
}

void Rover::drawSupport(double radius, const double start[3], const double end[3], int texture)
{
  if (texture == -1)
  {
    glBindTexture(GL_TEXTURE_2D, supportTexture); // Set to default support texture
  }
  else
  {
    glBindTexture(GL_TEXTURE_2D, texture); // Set to custom texture
  }
  glColor3f(1, 1, 1); // Set color to white to not affect texture color

  // Calculate the rotation angle and axis to align the cylinder
  double angle;
  double rotationAxis[3];
  Util::calculateRotation(start, end, angle, rotationAxis);

  // Direction vector from start to end
  double dirX = end[0] - start[0];
  double dirY = end[1] - start[1];
  double dirZ = end[2] - start[2];

  // Calculate the length of the cylinder
  double cylinderLength = sqrt(dirX * dirX + dirY * dirY + dirZ * dirZ);
  if (cylinderLength == 0.0)
    return; // Avoid drawing a zero-length cylinder

  // Save the current transformation matrix
  glPushMatrix();

  // Translate to the start position
  glTranslated(start[0], start[1], start[2]);

  // Rotate the cylinder to align with the direction vector
  if (angle != 0.0)
    glRotated(angle, rotationAxis[0], rotationAxis[1], rotationAxis[2]);

  // Set color for the support (e.g., brown)
  // glColor3f(0.54f, 0.47f, 0.3f); // Brown color

  // Define the number of segments for the cylinder
  int segments = 36; // More segments = smoother cylinder
  double step = 2.0 * Util::PI / segments;

  // Draw the cylinder sides using GL_QUAD_STRIP
  glBegin(GL_QUAD_STRIP);
  for (int i = 0; i <= segments; ++i)
  {
    double theta = i * step;
    double x = radius * cos(theta);
    double z = radius * sin(theta);

    // Compute the normal vector for lighting
    glNormal3d(cos(theta), 0.0, sin(theta));

    // Calculate texture coordinates
    double texCoord = static_cast<double>(i) / segments;

    glTexCoord2d(texCoord, 0.0);
    glVertex3d(x, 0.0, z); // Bottom vertex

    glTexCoord2d(texCoord, 1.0);
    glVertex3d(x, cylinderLength, z); // Top vertex
  }
  glEnd();

  // Draw the bottom cap using GL_TRIANGLE_FAN
  glBegin(GL_TRIANGLE_FAN);
  glNormal3d(0.0, -1.0, 0.0); // Normal pointing down
  glVertex3d(0.0, 0.0, 0.0);  // Center of the bottom cap
  for (int i = 0; i <= segments; ++i)
  {
    double theta = i * step;
    double x = radius * cos(theta);
    double z = radius * sin(theta);
    glVertex3d(x, 0.0, z);
  }
  glEnd();

  // Draw the top cap using GL_TRIANGLE_FAN
  glBegin(GL_TRIANGLE_FAN);
  glNormal3d(0.0, 1.0, 0.0);            // Normal pointing up
  glVertex3d(0.0, cylinderLength, 0.0); // Center of the top cap
  for (int i = 0; i <= segments; ++i)
  {
    double theta = i * step;
    double x = radius * cos(theta);
    double z = radius * sin(theta);
    glVertex3d(x, cylinderLength, z);
  }
  glEnd();

  // Restore the transformation matrix
  glPopMatrix();
}
