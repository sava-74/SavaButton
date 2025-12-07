#ifndef SavaButton_h
#define SavaButton_h

#include <Arduino.h>

// === КОНСТАНТЫ ПИНОВ ===
#ifdef INPUT
#undef INPUT
#endif

#define INPUT 1
#define PLUS  5
#define MINUS 9

// === КОДЫ СОБЫТИЙ (ВОЗВРАЩАЕМЫЕ ЗНАЧЕНИЯ) ===
#define BTN_NONE   0  // Ничего
#define BTN_CLICK  1  // Одиночный клик
#define BTN_LONG   2  // Долгое нажатие (сработало удержание)
#define BTN_DOUBLE 3  // Двойной клик
#define BTN_REPEAT 4  // Шаг авто-повтора

// === НАСТРОЙКИ ДЛЯ readSmart (АРГУМЕНТ 1 - ТИП КЛИКА) ===
#define SM_CLICK  0   // Обычный одиночный клик (сразу при отпускании)
#define SM_DOUBLE 1   // Режим с ожиданием двойного клика

// === НАСТРОЙКИ ДЛЯ readSmart (АРГУМЕНТ 2 - ТИП ПОВТОРА) ===
#define SM_REP_NONE 0 // Авто-повтор выключен
#define SM_REPEAT   1 // Простой авто-повтор (с постоянной скоростью)
#define SM_PROG     2 // Прогрессивный авто-повтор (ускорение х2, х4)

class SavaButton {
  public:
    SavaButton();

    /**
     * @brief Настройка пина. Вызывается как функция: btn(2, PLUS);
     * @param pin Номер пина
     * @param mode Режим: PLUS (подтяжка к плюсу INPUT_PULLUP), MINUS (подтяжка к минусу INPUT_PULLDOWN), INPUT(INPUT)
     */
    void operator()(uint8_t pin, uint8_t mode = PLUS);

    /**
     * @brief Установка глобального времени антидребезга для всех кнопок
     * @param ms Время в миллисекундах (по умолчанию 40)
     */
    static void bounceTime(uint8_t ms);

    /**
     * @brief Базовое чтение состояния с антидребезгом
     * @return true - нажата, false - отпущена
     */
    bool read();

    /**
     * @brief Чтение с детектором длинного нажатия
     * @param longMs Время удержания в мс (по умолчанию 800)
     * @return Код события: BTN_NONE, BTN_CLICK, BTN_LONG
     */
    uint8_t readLong(uint16_t longMs = 800);

    /**
     * @brief Умное чтение: Двойной клик + Авто-повтор (Прогрессивный)
     * 
     * @param clickMode Режим кликов: SM_CLICK (обычный) или SM_DOUBLE (ждать двойной)
     * @param repMode Режим повтора: SM_REP_NONE, SM_REPEAT (обычный), SM_PROG (ускорение)
     * @param longMs Время долгого нажатия / Базовая скорость повтора (по умолч. 800)
     * @param doubleMs Время ожидания второго клика (по умолч. 250)
     * 
     * @return Код события: BTN_NONE, BTN_CLICK, BTN_LONG, BTN_DOUBLE, BTN_REPEAT
     */
    uint8_t readSmart(uint8_t clickMode = SM_CLICK, 
                      uint8_t repMode = SM_REPEAT, 
                      uint16_t longMs = 800, 
                      uint16_t doubleMs = 250);

  private:
    uint8_t _pin;
    uint8_t _mode;
    
    // Антидребезг
    bool _state;           
    bool _lastReading;     
    uint32_t _timer;       
    static uint8_t _globalDebounceMs;

    // Логика Long/Smart
    uint32_t _pressStartTime; // Время начала нажатия
    bool _longTriggered;      // Флаг срабатывания Long
    bool _lastStableState;    // Предыдущее стабильное состояние
    
    // Логика Smart (Double + Repeat)
    bool _waitingDouble;      // Ждем ли мы второй клик прямо сейчас?
    uint32_t _waitDoubleTimer;// Таймер ожидания второго клика
    uint32_t _repeatTimer;    // Таймер для отсчета шагов повтора
    uint16_t _repCounter;     // Счетчик шагов для прогрессии
};

#endif