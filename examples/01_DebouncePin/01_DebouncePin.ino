#include "SavaButton.h"

// 1. Создаем объекты глобально (пустые)
SavaButton btn12;
SavaButton btn13;

void setup() {
  Serial.begin(115200);

  // 2. Меняем время дребезга для ВСЕХ кнопок (если надо, по умолчанию 40)
  SavaButton::bounceTime(50); 

  // 3. Инициализируем пины
  btn12(12);          // Пин 2, по умолчанию PLUS (5)
  btn13(13);   // Пин 3, режим MINUS (9)
}

void loop() {
  if (btn12.read()) {
    Serial.println("Кнопка 12 нажата");
  }
  
  if (btn13.read()) {
    Serial.println("Кнопка 13 нажата");
  }
}