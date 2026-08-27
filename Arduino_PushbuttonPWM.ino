#include <Arduino.h>
#include <EEPROM.h>

constexpr uint8_t BUTTON_PIN = 2;    // External 10k pulldown, button to +5V
constexpr uint8_t FAN_PWM_PIN = 9;   // OC1A (D9) for 25 kHz PWM (PC fan standard)
constexpr uint8_t EEPROM_ADDR = 0;
constexpr uint8_t SPEED_STEPS = 5;   // N speeds; must divide 100 (e.g. 5 => 20..100%)
constexpr unsigned long DEBOUNCE_MS = 40;

constexpr uint16_t PWM_TOP = 639;    // 16 MHz / (1 * (1 + 639)) = 25 kHz
static_assert(SPEED_STEPS > 0, "SPEED_STEPS must be > 0");
static_assert(100 % SPEED_STEPS == 0, "SPEED_STEPS must divide 100 equally");

uint8_t currentStep = 0;             // 0..SPEED_STEPS-1

uint8_t lastButtonReading = LOW;
uint8_t stableButtonState = LOW;
unsigned long lastDebounceTime = 0;

void setupFanPwm25kHz() {
  pinMode(FAN_PWM_PIN, OUTPUT);

  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;

  // Fast PWM, TOP = ICR1 (mode 14), non-inverting on OC1A (D9), prescaler = 1
  TCCR1A |= _BV(COM1A1) | _BV(WGM11);
  TCCR1B |= _BV(WGM13) | _BV(WGM12) | _BV(CS10);

  ICR1 = PWM_TOP;
}

void applyCurrentStep() {
  const uint8_t percent = ((static_cast<uint16_t>(currentStep) + 1U) * 100U) / SPEED_STEPS;
  const uint16_t compareValue = (static_cast<uint32_t>(PWM_TOP) * percent) / 100U;
  OCR1A = compareValue;
}

void loadStepFromEeprom() {
  uint8_t storedStep = EEPROM.read(EEPROM_ADDR);
  if (storedStep >= SPEED_STEPS) {
    storedStep = 0;
  }
  currentStep = storedStep;
}

void saveStepToEeprom() {
  EEPROM.update(EEPROM_ADDR, currentStep);
}

void setup() {
  pinMode(BUTTON_PIN, INPUT);
  setupFanPwm25kHz();
  loadStepFromEeprom();
  applyCurrentStep();
}

void loop() {
  const uint8_t reading = digitalRead(BUTTON_PIN);

  if (reading != lastButtonReading) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) >= DEBOUNCE_MS) {
    if (reading != stableButtonState) {
      stableButtonState = reading;

      if (stableButtonState == HIGH) {
        currentStep = (currentStep + 1U) % SPEED_STEPS;
        applyCurrentStep();
        saveStepToEeprom();
      }
    }
  }

  lastButtonReading = reading;
}
