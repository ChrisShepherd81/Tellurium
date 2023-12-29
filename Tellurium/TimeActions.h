#ifndef TIMEACTIONS_H
#define TIMEACTIONS_H

#include "Hardware.h"
#include <TimeLib.h>

void PrintTime()
{
  time_t t = now();
  Serial.println("Current time is " + String(hour(t)) + ":" + String(minute(t)) + ":" + String(second(t)) + " " + String(day(t)) + "." + String(month(t)) + "." + String(year(t)));
}

void SetTimeManually()
{
  Serial.println("Enter current date in format DD.MM.YYYY :");
  while (Serial.available() == 0) {}     //wait for data available
  String date_string = Serial.readString();  
  int day = date_string.substring(0,2).toInt();
  int month = date_string.substring(3,5).toInt();
  int year = date_string.substring(6,10).toInt();
  Serial.println("Setting date to " + String(day) + "." + String(month) + "." + String(year));
  
  Serial.println("Enter current time in format HH:MM:SS :");
  while (Serial.available() == 0) {}
  String time_string = Serial.readString();  
  int hour = time_string.substring(0,2).toInt();
  int minute = time_string.substring(3,5).toInt();
  int second = time_string.substring(6,8).toInt();  
  Serial.println("Setting time to " + String(hour) + ":" + String(minute) + ":" + String(second));
  
  SetArduinoTime(hour, minute, second, day, month, year);
}

#endif // TIMEACTIONS_H
