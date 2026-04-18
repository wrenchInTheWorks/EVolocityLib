#ifndef EVOLOCITY_CHASSIS_CONTROLLER_H
#define EVOLOCITY_CHASSIS_CONTROLLER_H

#include <Arduino.h>
#include <RF24.h>
#include <Servo.h>

class EVolocityChassisController {
public:
    // All pins default to the standard EVolocity motor board wiring.
    // Only change them if you have rewired your board.
    EVolocityChassisController(
        uint8_t cePin    = 7,
        uint8_t csnPin   = 9,
        uint8_t irqPin   = 2,   // RF24 IRQ → Arduino pin 2 (INT0)
        uint8_t servoPin = 8,
        uint8_t enaPin   = 3,   // L298N PWM speed
        uint8_t in1Pin   = 5,
        uint8_t in2Pin   = 6,
        uint8_t battPin  = A0,
        uint8_t ledPin   = A3
    );

    // Call once in setup()
    void begin();

    // ── Read values sent from the handheld controller ──────────────────────
    int  getSteeringAngle();      // steering angle in degrees (0 – 180)
    int  getMotorSpeed();         // motor speed (0 – 255)
    int  getMotorDirection();     // -1 = reverse, 0 = stop, 1 = forward

    // ── Send commands to the car ────────────────────────────────────────────
    void setSteering(int angle);              // 0 – 180 degrees
    void setMotor(int speed, int direction);  // speed 0-255, direction -1/0/1
    void stop();                              // cut motor instantly

    // ── Status helpers ──────────────────────────────────────────────────────
    bool isBatteryLow();           // true when battery needs charging
    bool isControllerConnected();  // true when radio packets are arriving

    // Internal — called by ISR callbacks. Not for use in student code.
    void _handleRadioIRQ();
    void _handleTimerOverflow();

private:
    struct Packet {
        int servoPos;   // 0 – 180
        int motorSpeed; // 0 – 255
        int motorDir;   // -1, 0, +1
    };

    void _processFlags();  // safe (non-ISR) work triggered by interrupt flags

    RF24  _radio;
    Servo _servo;

    uint8_t _irqPin, _servoPin, _enaPin, _in1Pin, _in2Pin, _battPin, _ledPin;

    volatile bool    _radioFlag;      // set by radio IRQ
    volatile bool    _battCheckDue;   // set by Timer2 every ~1 s
    volatile uint8_t _timerTicks;

    Packet        _packet;
    bool          _battLow;
    unsigned long _lastPacketMs;

    static const byte     _address[6];
    static const uint8_t  _battThreshold  = 211;   // ADC ~6.2 V
    static const uint8_t  _ticksPerSecond = 61;    // Timer2/1024 overflows ≈ 1 s
    static const uint16_t _timeoutMs      = 500;   // no packet → disconnected
};

#endif
