#include "SavaButton.h"

SavaButton btn1;

void setup() {
  Serial.begin(9600);
  
  // Настройка: пин 2, режим MINUS (кнопка на +5В)
  btn1(12); 
}

void loop() {
  // Читаем событие (время долгого нажатия 1000 мс)
  uint8_t event = btn1.readLong(1000);

  if (event == BTN_CLICK) {
    Serial.println("Клик!");
  }
  
  if (event == BTN_LONG) {
    Serial.println("Долгое удержание!");
  }
}