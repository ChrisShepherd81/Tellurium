#ifndef PLANET_H
#define PLANET_H

#include <Arduino.h>
#include <Stepper.h>

enum class PlanetType{Mercury, Venus, Earth, Mars, Invalid};

class Planet
{
public:
  Planet(String name, PlanetType type,
         int steps_per_360_degree,
         int pin_1, int pin_2, int pin_3, int pin_4,
         int reed_pin,
         unsigned int step_for_sidereal_orbit,
         unsigned long real_seconds_per_orbit);

  PlanetType getType() const;  
  String getName() const;
  unsigned long getSteps() const;
  void addSteps(int steps);
  
  unsigned int getPositionForCurrentTime() const;
  unsigned long getSecondsBetweenSteps() const;

  bool isReferencePositionReached();
  void resetSteps();

  void makeStep();
  void makeSteps(int steps);
  void setSpeed(long speed);
  void disableSpeedDelay();
  void stopMotor();

  void prepareFastRun();
  unsigned long getPosition() const;
  bool oldPositionReached() const;

private:
  String m_Name;
  PlanetType m_Type;
  Stepper m_Stepper;
  int m_ReedContact;
  unsigned int m_StepsPerOrbit;
  unsigned long m_RealSecondsPerOrbit;
  int m_Pins[4];
  
  unsigned long m_StepsMade = 0;
  unsigned long m_Position = 0;
  unsigned long m_FastRunOldPosition = 0;
};

#endif // PLANET_H
