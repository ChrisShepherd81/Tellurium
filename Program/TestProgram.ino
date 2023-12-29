#include "SolarSystem.h"
#include "Hardware.h"
#include "Helper.h"
#include "TimeActions.h"

void setup() {
  Serial.begin(9600);
  Serial.print("TestProgram Planetarium\n"
               "(c) 2021-2022 Astronomische Arbeitskreis Salzkammergut\n"
               "\n\n");

  FastForwardButton.begin();
  InitClokLEDs();
  SetUpSolarSystem();
}

void loop() {
  PrintQuery();

  switch(ReadIntFromSerial())
  {
  case 1:
    RunPlanet();
    break;
  case 2:
    RunAllPlanets();
    break;
  case 3:
    SearchForZeroPosition(); 
    break;
  case 4:
    SetTimeManually();
    PrintTime();
    break;
  case 5:
    PrintTime();
    break;
  case 6:
    ReadPin();
    break;
  case 7:
    SetTimeFromDCF();
    break;
  case 8:
    GoToStartPosition();
    break;
  case 9:
    MeasurePlanetSteps();  
    break;
  case 10:
    GoToCurrentDate();
    break;
  case 11:
    FastRun();
    break;
  default:
    Serial.println("No valid choice");
  }

  Serial.println("-----------------------");
}

void PrintQuery()
{
  ClearSerial();
  Serial.println("Choose test routine to run:");
  Serial.println("1: Run a single planet (speed=15RPM)");
  Serial.println("2: Run all planets (speed=30RPM)");
  Serial.println("3: Search planet's zero position");
  Serial.println("4: Set current time manually");
  Serial.println("5: Print Time");
  Serial.println("6: Read PIN");
  Serial.println("7: Set DCF77 Clock");
  Serial.println("8: Go to Start");
  Serial.println("9: Measure Steps per Planet");
  Serial.println("10: Got to current data");
  Serial.println("11: Fast run");
  Serial.println("Enter your choice:");
}

void FastRun()
{
  Serial.println("Fast run of solar system");
  Serial.println("Press a key to stop.");

  //Initalize planets
  for(int i=0; i < NUMBER_OF_PLANETS; ++i)
  {
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
  while(Serial.available() <= 0)
  {
    Mercury.makeStep();

    if(Mercury.getSteps() / mv_ratio >= Venus.getSteps() + 1)
      Venus.makeStep();

    if(Mercury.getSteps() / me_ratio >= Earth.getSteps() + 1)
      Earth.makeStep();

    if(Mercury.getSteps() / mm_ratio >= Mars.getSteps() + 1)
      Mars.makeStep();
  }

  Serial.println("Fast run stopped. Returning planets to old positions.");

  // Test if planet can be reset backwards.
  // Might not work because of backlash
  while(!AllPlanetsInOldPosition())
  {
    for(int i=0; i < NUMBER_OF_PLANETS; ++i)
    {
      Planet* planet = SolarSystem[i];
      planet->setSpeed(15);

      if(planet->oldPositionReached())
      {
        Serial.println("Planet reached old position: " + planet->getName());
      }
      else
      {
        Serial.println("Planet " + planet->getName() + " at position: " + String(planet->getPosition()));
        planet->makeSteps(-1);
      }
    }
  }  

  Serial.println("Old positions reached. Routine end reached.");
}

void GoToCurrentDate()
{
  Serial.println("Go to current date routine");
  Serial.println("Moving all planets to start position");
  GoToStartPosition();
  Serial.println("Start position reached");

  //Planet loop
  for(int i=0; i < NUMBER_OF_PLANETS; ++i)
  {
    Planet* planet = SolarSystem[i];

    //Check if planet reached reference position
    if(planet->isReferencePositionReached())
    {
      Serial.println(planet->getName() + " is in reference position");  
      planet->resetSteps();
    }
    else
    {
      Serial.println("FAIL: " + planet->getName() + " is not in reference position");  
      continue;
    }

    unsigned int position_for_planet = planet->getPositionForCurrentTime();
    Serial.println("Moving: " + planet->getName() + " to position " + String(position_for_planet));  

    planet->makeSteps(position_for_planet);
  }
}

void MeasurePlanetSteps()
{
  Serial.println("Measure Steps per planet routine");
  Serial.println("Moving all planets to start position");
  GoToStartPosition();
  Serial.println("Start position reached");

  int numberOfSpeeds = 5;
  int speed[numberOfSpeeds] = {5,10,15,20,25};

  int numberOfTestsToRun = 5;

  //Planet loop
  for(int i=0; i < NUMBER_OF_PLANETS; ++i)
  {
    Planet* planet = SolarSystem[i];

    //Check if planet reached reference position
    if(planet->isReferencePositionReached())
    {
      Serial.println(planet->getName() + " is in reference position");  
    }
    else
    {
      Serial.println("FAIL: " + planet->getName() + " is not in reference position");  
      continue;
    }

    //Speed Loop
    for(int i=0; i<numberOfSpeeds; ++i)
    {
      Serial.println("Set speed for " + planet->getName() + " to " + String(speed[i]) );
      planet->setSpeed(speed[i]);

      //Tests loop
      for(int j=1; j <= numberOfTestsToRun; ++j)
      {
        //Reset steps 
        planet->resetSteps();

        //Move away from reference position
        planet->makeSteps(20);

        do
        {
          if(Serial.available() > 1)
          {
            //Abort routine
            return;
          }

          planet->makeStep();
        }
        while(!planet->isReferencePositionReached());
        
        Serial.println("Nr. " + String(j) + ": " + planet->getName() + " made " + planet->getSteps() + " for speed " + String(speed[i]));
      }
    }    
  }
}

void ReadPin()
{
  Serial.println("Enter pin number:");
  int choose_pin = ReadIntFromSerial();
  Serial.println("Reading value from Pin: " + String(choose_pin));  
  
  pinMode(choose_pin, INPUT);
    
  while(Serial.available() <= 1)
  {
    Serial.println("Pin value: " + String(digitalRead(choose_pin)));
  }
}

void SearchForZeroPosition()
{
  Planet* planet = ChoosePlanet();
  if(!planet)
  {
    Serial.println("No valid choice");
    return;
  }

  Serial.println("Searching Zero Position for " + planet->getName());
  planet->setSpeed(10);
  planet->resetSteps(); 

  while(Serial.available() <= 1)
  {
    if(!planet->isReferencePositionReached())
    {
      planet->makeStep();
    }
    else
    {
        break;
    }
  }

  Serial.println("Terminating zero search ...");
}

void RunPlanet()
{
  Planet* choosenPlanet = ChoosePlanet();
  if(!choosenPlanet)
  {
    Serial.println("No valid choice");
    return;
  }

  RunPlanet(choosenPlanet);
}

Planet* ChoosePlanet()
{
  Serial.print("Enter planet:\n"
                 "1: Mercury\n"
                 "2: Venus\n"
                 "3: Earth\n"
                 "4: Mars\n"
                 "Enter your choice:\n");

  switch(ReadIntFromSerial())
  {
  case 1:
    return &Mercury;
    break;
  case 2:
    return &Venus;
    break;
  case 3:
    return &Earth;
    break;
  case 4:
    return &Mars;
    break;
  default:
    return nullptr;
  }
}

void RunPlanet(Planet* planet)
{
  Serial.println("Running planet " + planet->getName());
  Serial.println("Enter number of steps, 0 for continous run:");
  int steps_to_make = ReadIntFromSerial();  

  planet->setSpeed(15);
  while(Serial.available() <= 0)
  {
    if(steps_to_make == 0)
      planet->makeStep();
    else
    {
      planet->makeSteps(steps_to_make);
      break;
    }  
  }
  planet->stopMotor();
}

void RunAllPlanets()
{
  Serial.println("Running solar system");
  
  for(int i=0; i < NUMBER_OF_PLANETS; ++i)
  {
    Serial.println("Set speed for " + SolarSystem[i]->getName());
    SolarSystem[i]->setSpeed(30);
  }

  while(Serial.available() <= 0)
  {
    for(int i=0; i < NUMBER_OF_PLANETS; ++i)
    {
      SolarSystem[i]->makeStep();
    }
  }
}

void ReadButtonState()
{
  while(Serial.available() <= 0)
  {
    if(FastForwardButton.pressed())
    {
      Serial.println("FastForwardButton pressed");
    }
    else
    {
      Serial.println("FastForwardButton released");
    }
  }
}
