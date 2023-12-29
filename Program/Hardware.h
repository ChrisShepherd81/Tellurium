#ifndef HARDWARE_H
#define HARDWARE_H

#include "DCF77.h"

#include <Button.h>
#include <TimeLib.h>

#define BUTTON_PIN 53
Button FastForwardButton(BUTTON_PIN);

#define DCF_PIN 2
#define DCF_INTERRUPT 0
#define PIN_LED_DCF_RECEIVED 53
#define PIN_LED_TIME_SET 52

void InitClokLEDs()
{
  pinMode(PIN_LED_DCF_RECEIVED, OUTPUT);
  pinMode(PIN_LED_TIME_SET, OUTPUT);

  digitalWrite(PIN_LED_DCF_RECEIVED, LOW);
  digitalWrite(PIN_LED_TIME_SET, LOW);
}

void SetArduinoTime(time_t time)
{
  setTime(time);
  digitalWrite(PIN_LED_TIME_SET, HIGH);
}

void SetArduinoTime(int hour, int minute, int second, int day, int month, int year)
{
  setTime(hour, minute, second, day, month, year);
  digitalWrite(PIN_LED_TIME_SET, HIGH);
}

class DCFClock
{
public:
  DCFClock() : m_dcf(DCF_PIN,DCF_INTERRUPT)
  {
    m_dcf.Start();
  }

  ~DCFClock()
  {
     m_dcf.Stop();
  }

  bool UpdateTime()
  {
    time_t DCFtime = m_dcf.getTime();
    if (DCFtime!=0)
    {
      SetArduinoTime(DCFtime);
      return true;
    }   
    return false; 
  }

private:
  DCF77 m_dcf; 
};

#endif //HARDWARE_H
