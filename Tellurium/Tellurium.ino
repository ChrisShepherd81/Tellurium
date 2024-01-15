#include "SolarSystem.h"
#include "Hardware.h"
#include "Helper.h"
#include "TimeActions.h"
#include "SerialIO.h"

DCFClock dcf_clock;
bool FirstTimeInitialization = true;
bool DCFLedState = false;

void setup() {
  Serial.begin(9600);
  PrintWelcome();

  //FastForwardButton.begin();
  InitClockLEDs();

  SetUpSolarSystem();
  MoveAllPlanetsToReferencePositions();

  PrintOptions();
}

void HandleInput() {
  switch (ReadOption()) {
    case 1:
      SetTimeManually();
      [[fallthrough]];
    case 2:
      PrintTime();
      break;
    default:
      return;
  }

  PrintOptions();
}

void delayLoop(unsigned long loop_start) {
  unsigned long loop_now = millis();
  unsigned long loop_runtime = loop_now - loop_start;

  if (loop_runtime < 1000UL)
    delay(1000UL - loop_runtime);
}

void UpdateTimeAndLeds() {
  if (dcf_clock.UpdateTime()) {
    digitalWrite(PIN_LED_DCF_RECEIVED, HIGH);
  }
  else {
    digitalWrite(PIN_LED_DCF_RECEIVED, DCFLedState);
    DCFLedState = !DCFLedState;
  }
}

void loop() {
  unsigned long loop_start = millis();

  HandleInput();
  UpdateTimeAndLeds();

  if (FirstTimeInitialization && TimeIsSet) {
    MoveAllPlanetsToCurrentDate();
    FirstTimeInitialization = false;
  }
  else if(!FirstTimeInitialization && TimeIsSet) {
    MovePlanets();
  }
  
  delayLoop(loop_start);
}

void FastRun() {
  Serial.println("Fast run of solar system");
  Serial.println("Press a key to stop.");

  //Initalize planets
  for (int i = 0; i < NUMBER_OF_PLANETS; ++i) {
    Planet* planet = SolarSystem[i];
    planet->prepareFastRun();
    planet->resetSteps();
    planet->disableSpeedDelay();
  }

  //Speed of fast run depends on Mercury's speed
  Mercury.setSpeed(15);
  Serial.println("Planets initalized.");

  //run loop
  double mv_ratio = Venus.getSecondsBetweenSteps() / Mercury.getSecondsBetweenSteps();
  double me_ratio = Earth.getSecondsBetweenSteps() / Mercury.getSecondsBetweenSteps();
  double mm_ratio = Mars.getSecondsBetweenSteps() / Mercury.getSecondsBetweenSteps();
  while (Serial.available() <= 0) {
    Mercury.makeStep();

    if (Mercury.getSteps() / mv_ratio >= Venus.getSteps() + 1)
      Venus.makeStep();

    if (Mercury.getSteps() / me_ratio >= Earth.getSteps() + 1)
      Earth.makeStep();

    if (Mercury.getSteps() / mm_ratio >= Mars.getSteps() + 1)
      Mars.makeStep();
  }

  Serial.println("Fast run stopped. Returning planets to old positions.");

  // Test if planet can be reset backwards.
  // Might not work because of backlash
  while (!AllPlanetsInOldPosition()) {
    for (int i = 0; i < NUMBER_OF_PLANETS; ++i) {
      Planet* planet = SolarSystem[i];
      planet->setSpeed(15);

      if (planet->oldPositionReached()) {
        Serial.println("Planet reached old position: " + planet->getName());
      } else {
        Serial.println("Planet " + planet->getName() + " at position: " + String(planet->getPosition()));
        planet->makeSteps(-1);
      }
    }
  }

  Serial.println("Old positions reached. Routine end reached.");
}

void ReadButtonState() {
  while (Serial.available() <= 0) {
    if (FastForwardButton.pressed()) {
      Serial.println("FastForwardButton pressed");
    } else {
      Serial.println("FastForwardButton released");
    }
  }
}
