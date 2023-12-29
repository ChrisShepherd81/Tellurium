#include "Helper.h"

#include <Arduino.h>

void WaitForSerialInput()
{
  while(Serial.available() <= 0)
  {
    delay(10);
  }
}

void ClearSerial()
{
  while(Serial.available() > 0)
    Serial.read();
}

int ReadIntFromSerial()
{
  WaitForSerialInput();
  int value = Serial.parseInt();
  ClearSerial();
  return value;
}

bool PollPinHigh(int pin)
{
  for(int i=0; i < 100; ++i)
  {
    if(digitalRead(pin) == 1)
    {
      return true;
    }
  }
  return false;
}