#include "SavaButton.h"

// 1. Создаем объекты глобально
SavaButton btnSimple;
SavaButton btnComplex;

// Переменная которую будем менять кнопкой 2
int value = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("Start...");

  // 2. Настройка времени антидребезга (для всех кнопок сразу)
  SavaButton::bounceTime(50); 

  // 3. Инициализация пинов
  // Кнопка 1: Пин 2, ожидаем +5В (внешний резистор на землю)
  btnSimple(12); 

  // Кнопка 2: Пин 3, внутренняя подтяжка (замыкает на землю)
  btnComplex(15, PLUS);
}

void loop() {
  // === ЛОГИКА КНОПКИ 1 (Простая: readLong) ===
  // Аргумент: 1000 мс для долгого нажатия
  uint8_t s1 = btnSimple.readLong(1000);

  if (s1 == BTN_CLICK) {
    Serial.println("Btn1: Клик");
  }
  if (s1 == BTN_LONG) {
    Serial.println("Btn1: Долгое удержание");
  }


  // === ЛОГИКА КНОПКИ 2 (Умная: readSmart) ===
  // Аргументы:
  // 1. SM_DOUBLE - ждем двойной клик
  // 2. SM_PROG   - включить прогрессивный авто-повтор
  // 3. 600       - время удержания (оно же начальная скорость шагов)
  // 4. 250       - время ожидания второго клика
  
  uint8_t s2 = btnComplex.readSmart(SM_DOUBLE, SM_PROG, 600, 250);

  switch (s2) {
    case BTN_CLICK:
      Serial.println("Btn2: Одиночный (с задержкой)");
      break;
      
    case BTN_DOUBLE:
      Serial.println("Btn2: ДВОЙНОЙ КЛИК!");
      value = 0; // Сброс значения
      Serial.print("Value reset: "); Serial.println(value);
      break;

    case BTN_LONG:
      Serial.println("Btn2: Старт удержания...");
      break;

    case BTN_REPEAT:
      value++; // Увеличиваем значение
      Serial.print("Btn2: Шаг >> "); 
      Serial.println(value);
      break;
  }
}