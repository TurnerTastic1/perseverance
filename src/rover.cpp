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

void Rover::draw()
{
  buildBody();     // Build the rover's body
  buildSupports(); // Build the rover's supports
  buildWheels();   // Build the rover's wheels
  buildCamera();   // Build the rover's camera

  // Example: Draw a vertical support from (0, 0, 0) to (3, 3, 3)
  // double start1[3] = {0.0, 0.0, 0.0};
  // double end1[3] = {3.0, 3.0, 3.0};
  // drawSupport(3.0, start1, end1);

  // // 1. Horizontal Pipe along the X-axis
  // double horizontalStartX[3] = {-10.0, bodyPlacementHeight, 0.0}; // Start point
  // double horizontalEndX[3] = {10.0, bodyPlacementHeight, 0.0};    // End point
  // drawSupport(2.0, horizontalStartX, horizontalEndX);

  // // 2. Horizontal Pipe along the Z-axis
  // double horizontalStartZ[3] = {0.0, bodyPlacementHeight, -10.0}; // Start point
  // double horizontalEndZ[3] = {0.0, bodyPlacementHeight, 10.0};    // End point
  // drawSupport(2.0, horizontalStartZ, horizontalEndZ);

  // // **Additional Horizontal Pipes:**
  // // You can add more horizontal pipes at different heights or positions as needed.

  // // Example: Another horizontal pipe above the first one
  // double horizontalStartX2[3] = {-8.0, bodyPlacementHeight + 5.0, 0.0};
  // double horizontalEndX2[3] = {8.0, bodyPlacementHeight + 5.0, 0.0};
  // drawSupport(1.5, horizontalStartX2, horizontalEndX2);
}

void Rover::buildBody()
{
  // Setting the color to white for the rover body
  glColor3f(1.0f, 1.0f, 1.0f); // RGB (1.0, 1.0, 1.0) for white

  // Vertices for a cuboid centered at the origin, scaled by 'size'
  float vertices[8][3] = {
      {-0.75f * size, -0.25f * size + bodyPlacementHeight, -0.4f * size}, // Vertex 0
      {0.75f * size, -0.25f * size + bodyPlacementHeight, -0.4f * size},  // Vertex 1
      {0.75f * size, 0.25f * size + bodyPlacementHeight, -0.4f * size},   // Vertex 2
      {-0.75f * size, 0.25f * size + bodyPlacementHeight, -0.4f * size},  // Vertex 3
      {-0.75f * size, -0.25f * size + bodyPlacementHeight, 0.4f * size},  // Vertex 4
      {0.75f * size, -0.25f * size + bodyPlacementHeight, 0.4f * size},   // Vertex 5
      {0.75f * size, 0.25f * size + bodyPlacementHeight, 0.4f * size},    // Vertex 6
      {-0.75f * size, 0.25f * size + bodyPlacementHeight, 0.4f * size}    // Vertex 7
  };

  // Drawing the cuboid using quads
  glBegin(GL_QUADS);

  // Front face
  glVertex3fv(vertices[4]);
  glVertex3fv(vertices[5]);
  glVertex3fv(vertices[6]);
  glVertex3fv(vertices[7]);

  // Back face
  glVertex3fv(vertices[0]);
  glVertex3fv(vertices[1]);
  glVertex3fv(vertices[2]);
  glVertex3fv(vertices[3]);

  // Left face
  glVertex3fv(vertices[0]);
  glVertex3fv(vertices[4]);
  glVertex3fv(vertices[7]);
  glVertex3fv(vertices[3]);

  // Right face
  glVertex3fv(vertices[1]);
  glVertex3fv(vertices[5]);
  glVertex3fv(vertices[6]);
  glVertex3fv(vertices[2]);

  // Top face
  glVertex3fv(vertices[3]);
  glVertex3fv(vertices[2]);
  glVertex3fv(vertices[6]);
  glVertex3fv(vertices[7]);

  // Bottom face
  glVertex3fv(vertices[0]);
  glVertex3fv(vertices[1]);
  glVertex3fv(vertices[5]);
  glVertex3fv(vertices[4]);

  glEnd();
}

void Rover::buildWheels()
{
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
    glVertex3d(x, y, 0.0);                   // Bottom vertex
    glVertex3d(x, y, height);                // Top vertex
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
    glVertex3d(x, y, height / 1.5);          // Bottom vertex
    glVertex3d(x, y, height / 2);            // Top vertex
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
    glVertex3d(innerX, innerY, height / 2); // Inner point
    glVertex3d(outerX, outerY, height / 2); // Outer point
  }
  glEnd();
  glLineWidth(1.0f); // Reset line width to default

  glPopMatrix();
}

void Rover::buildCamera()
{
  // * Camera arm
  double cameraArmStart[3] = {0.62 * size, bodyPlacementHeight + (bodyPlacementHeight * 0.4), 0.27 * size}; // Start point
  double cameraArmEnd[3] = {0.62 * size, bodyPlacementHeight * 2.2, 0.27 * size};                           // End point
  drawSupport(0.8, cameraArmStart, cameraArmEnd);

  // * Camera
  // Set the color for the camera
  glColor3f(1.0f, 0.0f, 0.0f); // White color

  // Offset to make the camera sit directly on top of the arm
  double cubeSize = 0.4 * size; // Size of the small cube
  double centerX = cameraArmEnd[0];
  double centerY = cameraArmEnd[1] + (cubeSize / 2); // Position it on top of the arm
  double centerZ = cameraArmEnd[2];

  // Vertices for the small camera cube
  float vertices[8][3] = {
      {centerX - cubeSize / 2, centerY - cubeSize / 2, centerZ - cubeSize / 2}, // Vertex 0
      {centerX + cubeSize / 2, centerY - cubeSize / 2, centerZ - cubeSize / 2}, // Vertex 1
      {centerX + cubeSize / 2, centerY - cubeSize / 2, centerZ + cubeSize / 2}, // Vertex 2
      {centerX - cubeSize / 2, centerY - cubeSize / 2, centerZ + cubeSize / 2}, // Vertex 3
      {centerX - cubeSize / 2, centerY + cubeSize / 2, centerZ - cubeSize / 2}, // Vertex 4
      {centerX + cubeSize / 2, centerY + cubeSize / 2, centerZ - cubeSize / 2}, // Vertex 5
      {centerX + cubeSize / 2, centerY + cubeSize / 2, centerZ + cubeSize / 2}, // Vertex 6
      {centerX - cubeSize / 2, centerY + cubeSize / 2, centerZ + cubeSize / 2}  // Vertex 7
  };

  // Drawing the camera using quads
  glBegin(GL_QUADS);

  // Front face
  glVertex3fv(vertices[4]);
  glVertex3fv(vertices[5]);
  glVertex3fv(vertices[6]);
  glVertex3fv(vertices[7]);

  // Back face
  glVertex3fv(vertices[0]);
  glVertex3fv(vertices[1]);
  glVertex3fv(vertices[2]);
  glVertex3fv(vertices[3]);

  // Left face
  glVertex3fv(vertices[0]);
  glVertex3fv(vertices[4]);
  glVertex3fv(vertices[7]);
  glVertex3fv(vertices[3]);

  // Right face
  glVertex3fv(vertices[1]);
  glVertex3fv(vertices[5]);
  glVertex3fv(vertices[6]);
  glVertex3fv(vertices[2]);

  // Top face
  glVertex3fv(vertices[3]);
  glVertex3fv(vertices[2]);
  glVertex3fv(vertices[6]);
  glVertex3fv(vertices[7]);

  // Bottom face
  glVertex3fv(vertices[0]);
  glVertex3fv(vertices[1]);
  glVertex3fv(vertices[5]);
  glVertex3fv(vertices[4]);

  glEnd();
}

void Rover::buildSupports()
{
  // * First support
  double supportOneStart[3] = {-0.75 * size, bodyPlacementHeight * .68, 0.5 * size}; // Start point
  double supportOneEnd[3] = {-0.30 * size, bodyPlacementHeight * .68, 0.5 * size};   // End point
  drawSupport(1.3, supportOneStart, supportOneEnd);

  double supportOneStartA[3] = {-0.30 * size, bodyPlacementHeight * .68, 0.5 * size};
  double supportOneEndA[3] = {0.22 * size, bodyPlacementHeight, 0.5 * size};
  drawSupport(1.3, supportOneStartA, supportOneEndA);

  double supportOneStartB[3] = {0.22 * size, bodyPlacementHeight, 0.5 * size};
  double supportOneEndB[3] = {0.75 * size, bodyPlacementHeight * 0.68, 0.5 * size};
  drawSupport(1.3, supportOneStartB, supportOneEndB);

  double supportOneStartC[3] = {-0.30 * size, bodyPlacementHeight * 0.68, 0.5 * size};
  double supportOneEndC[3] = {0.05 * size, bodyPlacementHeight * 0.4, 0.5 * size};
  drawSupport(0.8, supportOneStartC, supportOneEndC);

  // * Second support
  double supportTwoStart[3] = {-0.75 * size, bodyPlacementHeight * .68, -0.5 * size}; // Start point
  double supportTwoEnd[3] = {-0.30 * size, bodyPlacementHeight * .68, -0.5 * size};   // End point
  drawSupport(1.3, supportTwoStart, supportTwoEnd);

  double supportTwoStartA[3] = {-0.30 * size, bodyPlacementHeight * .68, -0.5 * size};
  double supportTwoEndA[3] = {0.22 * size, bodyPlacementHeight, -0.5 * size};
  drawSupport(1.3, supportTwoStartA, supportTwoEndA);

  double supportTwoStartB[3] = {0.22 * size, bodyPlacementHeight, -0.5 * size};
  double supportTwoEndB[3] = {0.75 * size, bodyPlacementHeight * 0.68, -0.5 * size};
  drawSupport(1.3, supportTwoStartB, supportTwoEndB);

  double supportTwoStartC[3] = {-0.30 * size, bodyPlacementHeight * 0.68, -0.5 * size};
  double supportTwoEndC[3] = {0.05 * size, bodyPlacementHeight * 0.4, -0.5 * size};
  drawSupport(0.8, supportTwoStartC, supportTwoEndC);
}

void Rover::drawSupport(double radius, const double start[3], const double end[3])
{
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
  glColor3f(0.54f, 0.47f, 0.3f); // Brown color

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
