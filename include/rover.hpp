#ifndef ROVER_HPP
#define ROVER_HPP

class Rover
{
public:
  Rover();
  void draw(bool isDay);

  // Load textures
  void loadTextures();

private:
  double size;
  double bodyPlacementHeight;

  int bodyTexture, supportTexture, wheelTexture, drillTexture, drillBitTexture;

  void buildBody();
  void buildSupports();
  void buildWheels();
  void buildCamera(bool isDay);
  void buildArmDrill();
  void buildRearPowerSource();

  // Drawing methods
  void drawSupport(double radius, const double start[3], const double end[3], int texture);
  void drawWheel(double radius, double height);
};

#endif