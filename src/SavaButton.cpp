#include "SavaButton.h"

uint8_t SavaButton::_globalDebounceMs = 40;

// === Конструктор ===
SavaButton::SavaButton() {
    _pin = 0;
    _mode = PLUS;

    // Сброс всех переменных
    _state = false;
    _lastReading = false;
    _timer = 0;

    _pressStartTime = 0;
    _longTriggered = false;
    _lastStableState = false;

    _waitingDouble = false;
    _waitDoubleTimer = 0;
    _repeatTimer = 0;
    _repCounter = 0;

    // Инициализация настроек по умолчанию
    _longMs = 600;
    _clickMode = SM_CLICK;
    _repMode = SM_REPEAT;
    _smartLongMs = 600;
    _doubleMs = 250;
}

// === Установка времени антидребезга ===
void SavaButton::bounceTime(uint8_t ms) {
    _globalDebounceMs = ms;
}

// === Установка времени долгого нажатия для readLong ===
void SavaButton::setLong(uint16_t longMs) {
    _longMs = longMs;
}

// === Установка параметров для readSmart ===
void SavaButton::setSmart(uint8_t clickMode, uint8_t repMode, uint16_t longMs, uint16_t doubleMs) {
    _clickMode = clickMode;
    _repMode = repMode;
    _smartLongMs = longMs;
    _doubleMs = doubleMs;
}

// === Инициализация (оператор скобок) ===
void SavaButton::operator()(uint8_t pin, uint8_t mode) {
    _pin = pin;
    _mode = mode;
    pinMode(_pin, _mode);
}

// === Базовый read() ===
bool SavaButton::read() {
    bool current = digitalRead(_pin);

    if (_mode == PLUS) {
        current = !current;
    }

    if (current != _lastReading) {
        _timer = millis();
    }

    if ((millis() - _timer) > _globalDebounceMs) {
        if (current != _state) {
            _state = current;
        }
    }

    _lastReading = current;
    return _state;
}

// === readLong ===
uint8_t SavaButton::readLong() {
    bool currentState = read();
    uint8_t result = BTN_NONE;

    if (currentState && !_lastStableState) {
        _pressStartTime = millis();
        _longTriggered = false;
    }
    else if (currentState) {
        if (!_longTriggered && (millis() - _pressStartTime > _longMs)) {
            _longTriggered = true;
            result = BTN_LONG;
        }
    }
    else if (!currentState && _lastStableState) {
        if (!_longTriggered) {
            result = BTN_CLICK;
        }
    }

    _lastStableState = currentState;
    return result;
}

// === readSmart ===
uint8_t SavaButton::readSmart() {
    // 1. Читаем текущее состояние кнопки
    bool currentState = read();
    uint8_t result = BTN_NONE;

    // --- ОБРАБОТКА НАЖАТИЯ (ФРОНТ 0->1) ---
    if (currentState && !_lastStableState) {
        // Если мы ждали второй клик и нажали снова -> ЭТО ДВОЙНОЙ!
        if (_clickMode == SM_DOUBLE && _waitingDouble) {
            _waitingDouble = false; // Сброс ожидания
            _lastStableState = currentState; 
            _pressStartTime = millis(); 
            
            // Ставим флаг, будто долгое нажатие уже обработано.
            // Это заставит блок "ОТПУСКАНИЕ" проигнорировать поднятие пальца после двойного клика.
            _longTriggered = true; 

            return BTN_DOUBLE; 
        }

        // Обычное первое нажатие
        _pressStartTime = millis();
        _longTriggered = false;
        _repCounter = 0; // Сброс счетчика шагов
    }

    // --- ОБРАБОТКА УДЕРЖАНИЯ (1) ---
    else if (currentState) {
        // Проверка на Long Press
        if ((millis() - _pressStartTime > _smartLongMs)) {

            // Первое срабатывание удержания
            if (!_longTriggered) {
                _longTriggered = true;
                _waitingDouble = false; // Если держали долго, двойной клик отменяется
                _repeatTimer = millis(); // Старт таймера повторов

                // НОВАЯ ЛОГИКА: если включен авто-повтор, возвращаем BTN_CLICK
                if (_repMode != SM_REP_NONE) {
                    result = BTN_CLICK;  // Первый клик при удержании
                } else {
                    result = BTN_LONG;   // Долгое нажатие (только если нет повтора)
                }
            }
            // Если Long уже был, работает Авто-повтор
            else if (_repMode != SM_REP_NONE) {
                uint16_t interval = _smartLongMs; // Базовая скорость

                if (_repMode == SM_PROG) {
                    if (_repCounter > 15) interval = 50;      // Максимальная скорость
                    else if (_repCounter > 5) interval = 200; // Средняя скорость
                }

                // Проверка таймера повтора
                if (millis() - _repeatTimer > interval) {
                    _repeatTimer = millis();
                    _repCounter++;
                    result = BTN_CLICK;  // ИЗМЕНЕНО: возвращаем BTN_CLICK вместо BTN_REPEAT
                }
            }
        }
    }

    // --- ОБРАБОТКА ОТПУСКАНИЯ (ФРОНТ 1->0) ---
    else if (!currentState && _lastStableState) {
        // Если флаг _longTriggered стоит (значит был Long или Double), то игнорируем отпускание.
        if (!_longTriggered) {
            if (_clickMode == SM_DOUBLE) {
                // Если режим двойного клика -> не отправляем сразу, а ждем
                _waitingDouble = true;
                _waitDoubleTimer = millis();
            } else {
                // Если режим обычный -> сразу клик
                result = BTN_CLICK;
            }
        }
    }

    // --- РЕЖИМ ОЖИДАНИЯ ВТОРОГО КЛИКА (IDLE) ---
    if (!currentState && _clickMode == SM_DOUBLE && _waitingDouble) {
        // Если время ожидания вышло
        if (millis() - _waitDoubleTimer > _doubleMs) {
            _waitingDouble = false;
            result = BTN_CLICK; // Отправляем "отложенный" клик
        }
    }

    _lastStableState = currentState;
    return result;
}


