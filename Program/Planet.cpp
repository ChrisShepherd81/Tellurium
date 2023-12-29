#include "Planet.h"
#include "Helper.h"

#include <TimeLib.h>

Planet::Planet(String name, PlanetType type,
     int steps_per_360_degree,
     int pin_1, int pin_2, int pin_3, int pin_4,
     int reed_pin,
     unsigned int step_for_sidereal_orbit,
     unsigned long real_seconds_per_orbit)
  : m_Name(name)
  , m_Type(type)
  , m_Stepper(steps_per_360_degree, pin_1, pin_2, pin_3, pin_4)
  , m_ReedContact(reed_pin)
  , m_StepsPerOrbit(step_for_sidereal_orbit)
  , m_RealSecondsPerOrbit(real_seconds_per_orbit)
{
  pinMode(m_ReedContact, INPUT);
  m_Pins[0] = pin_1;
  m_Pins[1] = pin_2;
  m_Pins[2] = pin_3;
  m_Pins[3] = pin_4;
}

PlanetType Planet::getType() const
{
  return m_Type;
}

void Planet::prepareFastRun()
{
  m_FastRunOldPosition = m_Position;
}

bool Planet::oldPositionReached() const
{
  return m_FastRunOldPosition == m_Position;
}

unsigned long Planet::getPosition() const
{
  return m_Position;
}

unsigned long Planet::getSteps() const
{
  return m_StepsMade;
}

void Planet::resetSteps()
{
  m_StepsMade = 0;
}

void Planet::makeStep()
{
  makeSteps(1);
}

void Planet::makeSteps(int steps)
{
  //Reverse direction
  m_Stepper.step(-steps);
  addSteps(steps);
}

void Planet::addSteps(int steps)
{
   m_StepsMade += steps;
   m_Position = (m_Position + steps) % m_StepsPerOrbit;
}

void Planet::setSpeed(long speed)
{
  m_Stepper.setSpeed(speed);
}

void Planet::disableSpeedDelay()
{
  // Step delay is calucalted in Stepper.cpp like this:
  // unsigned long step_delay = 60L * 1000L * 1000L / number_of_steps=100 / speed;
  // So setting it to 1 us should more or less disable the delay
  setSpeed(600000L);  
}

unsigned long Planet::getSecondsBetweenSteps() const
{
  return m_RealSecondsPerOrbit / m_StepsPerOrbit;
}

unsigned int Planet::getPositionForCurrentTime() const
{
  unsigned long long seconds_for_current_position = now() % m_RealSecondsPerOrbit;
  return seconds_for_current_position / getSecondsBetweenSteps();
}

void Planet::stopMotor()
{
  for(int i=0; i<4; ++i)
  {
    digitalWrite(m_Pins[i], LOW);
  }
}

String Planet::getName() const
{
  return m_Name;
}

bool Planet::isReferencePositionReached()
{
  if(!PollPinHigh(m_ReedContact))
    return false;
  
  m_Position = 0;
  return true;
}
