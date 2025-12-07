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
}

// === Установка времени антидребезга ===
void SavaButton::bounceTime(uint8_t ms) {
    _globalDebounceMs = ms;
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

// === readLong (для совместимости) ===
uint8_t SavaButton::readLong(uint16_t longMs) {
    bool currentState = read();
    uint8_t result = BTN_NONE;

    if (currentState && !_lastStableState) {
        _pressStartTime = millis();
        _longTriggered = false;
    }
    else if (currentState) {
        if (!_longTriggered && (millis() - _pressStartTime > longMs)) {
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

// === readSmart (Новая функция) ===
uint8_t SavaButton::readSmart(uint8_t clickMode, uint8_t repMode, uint16_t longMs, uint16_t doubleMs) {
    // 1. Читаем физику
    bool currentState = read();
    uint8_t result = BTN_NONE;

    // --- ОБРАБОТКА НАЖАТИЯ (ФРОНТ 0->1) ---
    if (currentState && !_lastStableState) {
        // Если мы ждали второй клик и нажали снова -> ЭТО ДВОЙНОЙ!
        if (clickMode == SM_DOUBLE && _waitingDouble) {
            _waitingDouble = false; // Сброс ожидания
            _lastStableState = currentState; 
            _pressStartTime = millis(); 
            
            // ИСПРАВЛЕНИЕ: Ставим флаг, будто долгое нажатие уже обработано.
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
        if ((millis() - _pressStartTime > longMs)) {
            
            // Первое срабатывание Long
            if (!_longTriggered) {
                _longTriggered = true;
                _waitingDouble = false; // Если держали долго, двойной клик отменяется
                _repeatTimer = millis(); // Старт таймера повторов
                result = BTN_LONG;
            } 
            // Если Long уже был, работает Авто-повтор
            else if (repMode != SM_REP_NONE) {
                // ИСПРАВЛЕНИЕ: Твоя логика скоростей
                uint16_t interval = longMs; // Базовая скорость

                if (repMode == SM_PROG) {
                    if (_repCounter > 15) interval = 50;      // Максимальная скорость
                    else if (_repCounter > 5) interval = 200; // Средняя скорость
                }

                // Проверка таймера повтора
                if (millis() - _repeatTimer > interval) {
                    _repeatTimer = millis();
                    _repCounter++;
                    result = BTN_REPEAT;
                }
            }
        }
    }

    // --- ОБРАБОТКА ОТПУСКАНИЯ (ФРОНТ 1->0) ---
    else if (!currentState && _lastStableState) {
        // Если флаг _longTriggered стоит (значит был Long или Double), то игнорируем отпускание.
        if (!_longTriggered) {
            if (clickMode == SM_DOUBLE) {
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
    if (!currentState && clickMode == SM_DOUBLE && _waitingDouble) {
        // Если время ожидания вышло
        if (millis() - _waitDoubleTimer > doubleMs) {
            _waitingDouble = false;
            result = BTN_CLICK; // Отправляем "отложенный" клик
        }
    }

    _lastStableState = currentState;
    return result;
}


