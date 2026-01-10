#ifndef SavaButton_h
#define SavaButton_h

/**
 * SavaButton Library v2.0.1
 * Автор: SavaLab (sava-74@inbox.ru)
 */

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
     * @brief Настройка времени долгого нажатия для readLong
     * @param longMs Время удержания в мс (по умолчанию 600)
     */
    void setLong(uint16_t longMs = 600);

    /**
     * @brief Настройка параметров для readSmart
     * @param clickMode Режим кликов: SM_CLICK или SM_DOUBLE (по умолчанию SM_CLICK)
     * @param repMode Режим повтора: SM_REP_NONE, SM_REPEAT, SM_PROG (по умолчанию SM_REPEAT)
     * @param longMs Время долгого нажатия / Базовая скорость повтора (по умолчанию 600)
     * @param doubleMs Время ожидания второго клика (по умолчанию 250)
     */
    void setSmart(uint8_t clickMode = SM_CLICK,
                  uint8_t repMode = SM_REPEAT,
                  uint16_t longMs = 600,
                  uint16_t doubleMs = 250);

    /**
     * @brief Базовое чтение состояния с антидребезгом
     * @return true - нажата, false - отпущена
     */
    bool read();

    /**
     * @brief Чтение с детектором длинного нажатия
     * @return Код события: BTN_NONE, BTN_CLICK, BTN_LONG
     * @note Используйте setLong() для настройки времени удержания
     */
    uint8_t readLong();

    /**
     * @brief Умное чтение: Двойной клик + Авто-повтор (Прогрессивный)
     *
     * @param repeat Включить авто-повтор (по умолчанию true). При false удержание возвращает BTN_NONE
     * @return Код события: BTN_NONE, BTN_CLICK, BTN_LONG, BTN_DOUBLE
     * @note BTN_LONG возвращается только если repMode == SM_REP_NONE
     * @note Если включен авто-повтор (SM_REPEAT/SM_PROG), то при удержании возвращается BTN_CLICK многократно
     * @note При repeat == false: короткое нажатие = BTN_CLICK, удержание = BTN_NONE
     */
    uint8_t readSmart(bool repeat = true);

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

    // Сохраненные настройки для readLong
    uint16_t _longMs;         // Время долгого нажатия

    // Сохраненные настройки для readSmart
    uint8_t _clickMode;       // Режим кликов (SM_CLICK / SM_DOUBLE)
    uint8_t _repMode;         // Режим повтора (SM_REP_NONE / SM_REPEAT / SM_PROG)
    uint16_t _smartLongMs;    // Время долгого нажатия для Smart
    uint16_t _doubleMs;       // Время ожидания второго клика
};

#endif