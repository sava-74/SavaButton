#include "SavaButton.h"

// 1. Создаем объекты глобально
SavaButton btnSimple;
SavaButton btnComplex;

// Переменная которую будем менять кнопкой 2
int value = 0;
bool flagR = 0;


void setup() {
  Serial.begin(115200);
  Serial.println("Start...");

  // 2. Настройка времени антидребезга (для всех кнопок сразу)
  SavaButton::bounceTime(50);

  // 3. Инициализация пинов
  // Кнопка 1: Пин 12, режим PLUS (по умолчанию)
  btnSimple(12);
  // Настройка времени долгого нажатия 1000 мс
  btnSimple.setLong(1000);

  // Кнопка 2: Пин 13, режим PLUS
  btnComplex(13, PLUS);
  // Настройка параметров Smart:
  // SM_DOUBLE - ждем двойной клик
  // SM_PROG - включить прогрессивный авто-повтор
  // 600 - время удержания (оно же начальная скорость шагов)
  // 250 - время ожидания второго клика
  btnComplex.setSmart(SM_DOUBLE, SM_PROG, 600, 250);
}

void loop() {
  // === ЛОГИКА КНОПКИ 1 (Простая: readLong) ===
  // Используем настройки из setup() - БЕЗ аргументов!
  uint8_t s1 = btnSimple.readLong();

  if (s1 == BTN_CLICK) {
    Serial.println("Btn1: Клик");
  }
  if (s1 == BTN_LONG) {
    flagR = !flagR;
    Serial.print("разрешить повтор = ");Serial.println(flagR);
  }


  // === ЛОГИКА КНОПКИ 2 (Умная: readSmart) ===
  // Используем настройки из setup() - БЕЗ аргументов!
  // ВАЖНО: Т.к. включен SM_PROG (авто-повтор), BTN_LONG не возвращается!
  // Вместо этого при удержании многократно возвращается BTN_CLICK
  uint8_t s2 = btnComplex.readSmart(flagR);

  switch (s2) {
    case BTN_CLICK:
      value++; // Увеличиваем значение (работает и для одиночного клика, и для повтора!)
      Serial.print("Btn2: Клик >> ");
      Serial.println(value);
      break;

    case BTN_DOUBLE:
      Serial.println("Btn2: ДВОЙНОЙ КЛИК!");
      value = 0; // Сброс значения
      Serial.print("Сброс значения: ");
      Serial.println(value);
      break;
  }
}