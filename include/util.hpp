#ifndef UTIL_HPP
#define UTIL_HPP

class Util
{
public:
  // Constants
  static const double PI;

  static void ErrCheck(const char *where);
  static void Fatal(const char *format, ...);
  static void Print(const char *format, ...);
  static void Vertex(double th, double ph);
  static int LoadTexBMP(const char *file);

  static double degToRad(double degrees);
  static void calculateRotation(const double start[3], const double end[3], double &angle, double rotationAxis[3]);
};

#endif