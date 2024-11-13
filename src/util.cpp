#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <cmath>
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
//  Default resolution
//  For Retina displays compile with -DRES=2
#ifndef RES
#define RES 1
#endif

// Cosine and Sine in degrees
#define Cos(x) (cos((x) * 3.14159265 / 180))
#define Sin(x) (sin((x) * 3.14159265 / 180))

// Constants
const double Util::PI = 3.14159265358979323846;

/*
 *  Check for OpenGL errors
 */
void Util::ErrCheck(const char *where)
{
  int err = glGetError();
  if (err)
    fprintf(stderr, "ERROR: %s [%s]\n", gluErrorString(err), where);
}

/*
 *  Convenience routine to output an error message and exit  - from ex9
 */
void Util::Fatal(const char *format, ...)
{
  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);
  exit(1);
}

/*
 *  Convenience routine to output raster text
 *  Use VARARGS to make this more flexible
 */
#define LEN 8192 //  Maximum length of text string
void Util::Print(const char *format, ...)
{
  char buf[LEN];
  char *ch = buf;
  va_list args;
  //  Turn the parameters into a character string
  va_start(args, format);
  vsnprintf(buf, LEN, format, args);
  va_end(args);
  //  Display the characters one at a time at the current raster position
  while (*ch)
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *ch++);
}

/*
 *  Draw vertex in polar coordinates with normal
 */
void Util::Vertex(double th, double ph)
{
  double x = Sin(th) * Cos(ph);
  double y = Cos(th) * Cos(ph);
  double z = Sin(ph);
  //  For a sphere at the origin, the position
  //  and normal vectors are the same
  glNormal3d(x, y, z);
  glVertex3d(x, y, z);
}

static void Reverse(void *x, const int n)
{
  char *ch = (char *)x;
  for (int k = 0; k < n / 2; k++)
  {
    char tmp = ch[k];
    ch[k] = ch[n - 1 - k];
    ch[n - 1 - k] = tmp;
  }
}

//
//  Load texture from BMP file
//
int Util::LoadTexBMP(const char *file)
{
  //  Open file
  FILE *f = fopen(file, "rb");
  if (!f)
    Fatal("Cannot open file %s\n", file);
  //  Check image magic
  unsigned short magic;
  if (fread(&magic, 2, 1, f) != 1)
    Fatal("Cannot read magic from %s\n", file);
  if (magic != 0x4D42 && magic != 0x424D)
    Fatal("Image magic not BMP in %s\n", file);
  //  Read header
  unsigned int dx, dy, off, k; // Image dimensions, offset and compression
  unsigned short nbp, bpp;     // Planes and bits per pixel
  if (fseek(f, 8, SEEK_CUR) || fread(&off, 4, 1, f) != 1 ||
      fseek(f, 4, SEEK_CUR) || fread(&dx, 4, 1, f) != 1 || fread(&dy, 4, 1, f) != 1 ||
      fread(&nbp, 2, 1, f) != 1 || fread(&bpp, 2, 1, f) != 1 || fread(&k, 4, 1, f) != 1)
    Fatal("Cannot read header from %s\n", file);
  //  Reverse bytes on big endian hardware (detected by backwards magic)
  if (magic == 0x424D)
  {
    Reverse(&off, 4);
    Reverse(&dx, 4);
    Reverse(&dy, 4);
    Reverse(&nbp, 2);
    Reverse(&bpp, 2);
    Reverse(&k, 4);
  }
  //  Check image parameters
  unsigned int max;
  glGetIntegerv(GL_MAX_TEXTURE_SIZE, (int *)&max);
  if (dx < 1 || dx > max)
    Fatal("%s image width %d out of range 1-%d\n", file, dx, max);
  if (dy < 1 || dy > max)
    Fatal("%s image height %d out of range 1-%d\n", file, dy, max);
  if (nbp != 1)
    Fatal("%s bit planes is not 1: %d\n", file, nbp);
  if (bpp != 24)
    Fatal("%s bits per pixel is not 24: %d\n", file, bpp);
  if (k != 0)
    Fatal("%s compressed files not supported\n", file);
#ifndef GL_VERSION_2_0
  //  OpenGL 2.0 lifts the restriction that texture size must be a power of two
  for (k = 1; k < dx; k *= 2)
    ;
  if (k != dx)
    Fatal("%s image width not a power of two: %d\n", file, dx);
  for (k = 1; k < dy; k *= 2)
    ;
  if (k != dy)
    Fatal("%s image height not a power of two: %d\n", file, dy);
#endif

  //  Allocate image memory
  unsigned int size = 3 * dx * dy;
  unsigned char *image = (unsigned char *)malloc(size);
  if (!image)
    Fatal("Cannot allocate %d bytes of memory for image %s\n", size, file);
  //  Seek to and read image
  if (fseek(f, off, SEEK_SET) || fread(image, size, 1, f) != 1)
    Fatal("Error reading data from image %s\n", file);
  fclose(f);
  //  Reverse colors (BGR -> RGB)
  for (k = 0; k < size; k += 3)
  {
    unsigned char temp = image[k];
    image[k] = image[k + 2];
    image[k + 2] = temp;
  }

  //  Sanity check
  ErrCheck("LoadTexBMP");
  //  Generate 2D texture
  unsigned int texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  //  Copy image
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, dx, dy, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
  if (glGetError())
    Fatal("Error in glTexImage2D %s %dx%d\n", file, dx, dy);
  //  Scale linearly when image size doesn't match
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  //  Free image memory
  free(image);
  //  Return texture name
  return texture;
}

// Utility function to convert degrees to radians
double Util::degToRad(double degrees)
{
  return degrees * Util::PI / 180.0;
}

void Util::calculateRotation(const double start[3], const double end[3], double &angle, double rotationAxis[3])
{
  // Direction vector from start to end
  double dirX = end[0] - start[0];
  double dirY = end[1] - start[1];
  double dirZ = end[2] - start[2];

  // Length of the direction vector
  double length = sqrt(dirX * dirX + dirY * dirY + dirZ * dirZ);
  if (length == 0.0)
  {
    angle = 0.0;
    rotationAxis[0] = 0.0;
    rotationAxis[1] = 1.0;
    rotationAxis[2] = 0.0;
    return;
  }

  // Normalize the direction vector
  double normX = dirX / length;
  double normY = dirY / length;
  double normZ = dirZ / length;

  // Y-axis unit vector
  double yAxisX = 0.0;
  double yAxisY = 1.0;
  double yAxisZ = 0.0;

  // Calculate the angle between the direction vector and the Y-axis
  double dotProduct = normX * yAxisX + normY * yAxisY + normZ * yAxisZ;
  // Clamp the dot product to avoid numerical errors
  if (dotProduct > 1.0)
    dotProduct = 1.0;
  if (dotProduct < -1.0)
    dotProduct = -1.0;
  angle = acos(dotProduct) * (180.0 / Util::PI); // Convert to degrees

  // Calculate the rotation axis using cross product of Y-axis and direction vector
  rotationAxis[0] = yAxisY * normZ - yAxisZ * normY;
  rotationAxis[1] = yAxisZ * normX - yAxisX * normZ;
  rotationAxis[2] = yAxisX * normY - yAxisY * normX;

  // Handle the case when the direction is parallel to the Y-axis
  double axisLength = sqrt(rotationAxis[0] * rotationAxis[0] + rotationAxis[1] * rotationAxis[1] + rotationAxis[2] * rotationAxis[2]);
  if (axisLength == 0.0)
  {
    // The direction is parallel to Y-axis
    rotationAxis[0] = 1.0;
    rotationAxis[1] = 0.0;
    rotationAxis[2] = 0.0;
    angle = (normY >= 0.0) ? 0.0 : 180.0;
  }
  else
  {
    // Normalize rotation axis
    rotationAxis[0] /= axisLength;
    rotationAxis[1] /= axisLength;
    rotationAxis[2] /= axisLength;
  }
}