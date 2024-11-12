#ifndef UTIL_HPP
#define UTIL_HPP

class Util
{
public:
  static void ErrCheck(const char *where);
  static void Fatal(const char *format, ...);
  static void Print(const char *format, ...);
  static void Vertex(double th, double ph);
  static int LoadTexBMP(const char *file);
};

#endif