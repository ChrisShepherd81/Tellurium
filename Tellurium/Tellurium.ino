#include "SolarSystem.h"
#include "Hardware.h"
#include "Helper.h"
#include "TimeActions.h"

DCFClock dcf_clock;
bool FirstTimeInitialization = true;

void setup() {
  Serial.begin(9600);
  Serial.print("Tellurium Planetarium\n"
               "(c) 2021-2023 Astronomischer Arbeitskreis Salzkammergut\n"
               "\n\n");

  //FastForwardButton.begin();
  InitClockLEDs();

  SetUpSolarSystem();
  MoveAllPlanetsToReferencePositions();

  PrintOptions();
}

void PrintOptions() {
  ClearSerial();
  Serial.println("Choose an option");
  Serial.println("1: Set Time");
  Serial.println("2: Print Time");
  Serial.println("Enter your choice:");
}

int CheckIfOptionsWasChoosen() {
  if (Serial.available() <= 0)
    return 0;

  int value = Serial.parseInt();
  ClearSerial();
  return value;
}

void loop() {
  unsigned long loop_start = millis();

  switch (CheckIfOptionsWasChoosen()) {
    case 1:
      SetTimeManually();
      // fall trough
    case 2:
      PrintTime();
      break;  
    default:
      break;
  }

  if (dcf_clock.UpdateTime())
    digitalWrite(PIN_LED_DCF_RECEIVED, HIGH);
  else
    digitalWrite(PIN_LED_DCF_RECEIVED, LOW);

  if(FirstTimeInitialization && TimeIsSet)
  {
    MoveAllPlanetsToCurrentDate();
    FirstTimeInitialization = false;
  }

  unsigned long loop_now = millis();
  unsigned long loop_runtime = loop_now - loop_start;
  
  if (loop_runtime < 1000UL) 
    delay(1000UL - loop_runtime);
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

void MoveAllPlanetsToCurrentDate() {
  Serial.println("Go to current date routine");
  Serial.println("Moving all planets to start position");
  MoveAllPlanetsToReferencePositions();
  Serial.println("Start position reached");

  //Planet loop
  for (int i = 0; i < NUMBER_OF_PLANETS; ++i) {
    Planet* planet = SolarSystem[i];

    //Check if planet reached reference position
    if (planet->isReferencePositionReached()) {
      Serial.println(planet->getName() + " is in reference position");
      planet->resetSteps();
    } else {
      Serial.println("FAIL: " + planet->getName() + " is not in reference position");
      continue;
    }

    unsigned int position_for_planet = planet->getPositionForCurrentTime();
    Serial.println("Moving: " + planet->getName() + " to position " + String(position_for_planet));

    planet->makeSteps(position_for_planet);
  }
}

void ReadPin() {
  Serial.println("Enter pin number:");
  int choose_pin = ReadIntFromSerial();
  Serial.println("Reading value from Pin: " + String(choose_pin));

  pinMode(choose_pin, INPUT);

  while (Serial.available() <= 1) {
    Serial.println("Pin value: " + String(digitalRead(choose_pin)));
  }
}

void RunAllPlanets() {
  Serial.println("Running solar system");

  for (int i = 0; i < NUMBER_OF_PLANETS; ++i) {
    Serial.println("Set speed for " + SolarSystem[i]->getName());
    SolarSystem[i]->setSpeed(30);
  }

  while (Serial.available() <= 0) {
    for (int i = 0; i < NUMBER_OF_PLANETS; ++i) {
      SolarSystem[i]->makeStep();
    }
  }
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
