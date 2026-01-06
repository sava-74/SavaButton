#include "SavaButton.h"

SavaButton btn1;

void setup() {
  Serial.begin(9600);

  // Настройка: пин 12, режим PLUS (по умолчанию)
  btn1(12);

  // Настройка времени долгого нажатия 1000 мс
  btn1.setLong(1000);
}

void loop() {
  // Читаем событие (без аргументов!)
  uint8_t event = btn1.readLong();

  if (event == BTN_CLICK) {
    Serial.println("Клик!");
  }

  if (event == BTN_LONG) {
    Serial.println("Долгое удержание!");
  }
}