#ifndef ROVER_HPP
#define ROVER_HPP

class Rover
{
public:
  Rover();
  void draw();

private:
  double size;
  double bodyPlacementHeight;

  void buildBody();
  void buildSupports();
  void buildWheels();
  void buildCamera();
  void buildArm();

  // Drawing methods
  void drawSupport(double radius, const double start[3], const double end[3]);
  void drawWheel(double radius, double height);
};

#endif