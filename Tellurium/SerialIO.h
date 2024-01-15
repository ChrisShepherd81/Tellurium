#ifndef SERIALIO_H
#define SERIALIO_H

void PrintWelcome() {
 Serial.print("Tellurium Planetarium\n"
               "(c) 2021-2024 Astronomischer Arbeitskreis Salzkammergut\n"
               "\n\n");
}

int ReadOption() {
  if (Serial.available() <= 0)
    return 0;

  int value = Serial.parseInt();
  ClearSerial();
  return value;
}

void PrintOptions() {
  ClearSerial();
  Serial.println("Choose an option");
  Serial.println("1: Set Time");
  Serial.println("2: Print Time");
  Serial.println("Enter your choice:");
}

#endif //SERIALIO_H
