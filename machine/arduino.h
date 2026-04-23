#ifndef ARDUINO_H
#define ARDUINO_H

// Arduino integration declarations are implemented in arduino.cpp.
// The machine class methods remain declared in machine.h.

enum class ThermalProfileMode {
  StandardA = 0,
  HotEnvironmentB = 1,
  AdaptiveC = 2,
};

#endif // ARDUINO_H
