#ifndef HELPER_H
#define HELPER_H

void WaitForSerialInput();

void ClearSerial();

int ReadIntFromSerial();

bool PollPinHigh(int pin);

#endif // HELPER_H
