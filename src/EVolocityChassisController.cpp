#include "EVolocityChassisController.h"

// ── Static data ────────────────────────────────────────────────────────────

const byte EVolocityChassisController::_address[6] = "00001";

// Global instance pointer — used by the two ISR free functions below.
static EVolocityChassisController* _instance = nullptr;

// Radio IRQ fires (FALLING) when a packet arrives.
static void _radioISR() {
    if (_instance) _instance->_handleRadioIRQ();
}

// Timer2 overflow fires every ~16.4 ms (16 MHz, prescaler 1024).
ISR(TIMER2_OVF_vect) {
    if (_instance) _instance->_handleTimerOverflow();
}

// ── Constructor ────────────────────────────────────────────────────────────

EVolocityChassisController::EVolocityChassisController(
    uint8_t cePin, uint8_t csnPin, uint8_t irqPin,
    uint8_t servoPin, uint8_t enaPin, uint8_t in1Pin, uint8_t in2Pin,
    uint8_t battPin, uint8_t ledPin)
    : _radio(cePin, csnPin),
      _irqPin(irqPin), _servoPin(servoPin),
      _enaPin(enaPin), _in1Pin(in1Pin), _in2Pin(in2Pin),
      _battPin(battPin), _ledPin(ledPin),
      _radioFlag(false), _battCheckDue(false), _timerTicks(0),
      _battLow(false), _lastPacketMs(0)
{
    _packet = {90, 0, 0};  // safe defaults: centred steering, motor off
}

// ── begin() ────────────────────────────────────────────────────────────────

void EVolocityChassisController::begin() {
    _instance = this;

    // Servo
    _servo.attach(_servoPin);
    _servo.write(90);  // centre

    // Motor driver
    pinMode(_enaPin, OUTPUT);
    pinMode(_in1Pin, OUTPUT);
    pinMode(_in2Pin, OUTPUT);
    analogWrite(_enaPin, 0);
    digitalWrite(_in1Pin, LOW);
    digitalWrite(_in2Pin, LOW);

    // Battery LED
    pinMode(_ledPin, OUTPUT);
    digitalWrite(_ledPin, LOW);

    // Radio
    if (!_radio.begin()) {
        // Flash the LED rapidly to signal a hardware fault.
        while (true) {
            digitalWrite(_ledPin, HIGH); delay(100);
            digitalWrite(_ledPin, LOW);  delay(100);
        }
    }
    // Only trigger IRQ for RX — ignore TX events.
    _radio.maskIRQ(true, true, false);
    _radio.openReadingPipe(0, _address);
    _radio.setPALevel(RF24_PA_LOW);
    _radio.startListening();

    // Attach radio IRQ on falling edge (pin goes LOW when packet arrives).
    pinMode(_irqPin, INPUT);
    attachInterrupt(digitalPinToInterrupt(_irqPin), _radioISR, FALLING);

    // Timer2: normal mode, prescaler 1024 → overflow every ~16.4 ms.
    TCCR2A = 0;
    TCCR2B = (1 << CS22) | (1 << CS21) | (1 << CS20);
    TIMSK2 = (1 << TOIE2);
}

// ── ISR handlers ───────────────────────────────────────────────────────────

void EVolocityChassisController::_handleRadioIRQ() {
    _radioFlag = true;
    // Actual SPI read happens in _processFlags() (non-ISR context).
}

void EVolocityChassisController::_handleTimerOverflow() {
    if (++_timerTicks >= _ticksPerSecond) {
        _timerTicks    = 0;
        _battCheckDue  = true;
    }
}

// ── Internal flag processing ───────────────────────────────────────────────

void EVolocityChassisController::_processFlags() {
    // Process incoming radio packet.
    if (_radioFlag) {
        _radioFlag = false;
        bool txOk, txFail, rxReady;
        _radio.whatHappened(txOk, txFail, rxReady);
        if (rxReady && _radio.available()) {
            _radio.read(&_packet, sizeof(_packet));
            _lastPacketMs = millis();
        }
    }

    // Check battery level (~once per second).
    if (_battCheckDue) {
        _battCheckDue = false;
        _battLow = (analogRead(_battPin) < _battThreshold);
        digitalWrite(_ledPin, _battLow ? HIGH : LOW);
    }
}

// ── Student-facing getters ─────────────────────────────────────────────────

int EVolocityChassisController::getSteeringAngle() {
    _processFlags();
    return constrain(_packet.servoPos, 0, 180);
}

int EVolocityChassisController::getMotorSpeed() {
    _processFlags();
    return constrain(_packet.motorSpeed, 0, 255);
}

int EVolocityChassisController::getMotorDirection() {
    _processFlags();
    return constrain(_packet.motorDir, -1, 1);
}

// ── Student-facing setters ─────────────────────────────────────────────────

void EVolocityChassisController::setSteering(int angle) {
    _processFlags();
    _servo.write(constrain(angle, 0, 180));
}

void EVolocityChassisController::setMotor(int speed, int direction) {
    _processFlags();
    speed     = constrain(speed, 0, 255);
    direction = constrain(direction, -1, 1);

    if (direction == 1) {
        digitalWrite(_in1Pin, HIGH);
        digitalWrite(_in2Pin, LOW);
    } else if (direction == -1) {
        digitalWrite(_in1Pin, LOW);
        digitalWrite(_in2Pin, HIGH);
    } else {
        digitalWrite(_in1Pin, LOW);
        digitalWrite(_in2Pin, LOW);
    }
    analogWrite(_enaPin, speed);
}

void EVolocityChassisController::stop() {
    setMotor(0, 0);
}

// ── Status helpers ─────────────────────────────────────────────────────────

bool EVolocityChassisController::isBatteryLow() {
    return _battLow;
}

bool EVolocityChassisController::isControllerConnected() {
    return (millis() - _lastPacketMs) < _timeoutMs;
}
