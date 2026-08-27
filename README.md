# Arduino Pushbutton PWM Fan Control (Arduino Nano)

This project controls a 4-wire PC fan PWM input using an Arduino Nano and one pushbutton.

## Features

- Pushbutton on **D2** cycles fan speed through **N** equal steps.
- Default `SPEED_STEPS = 5` gives: **20%, 40%, 60%, 80%, 100%**.
- After the highest step, next press wraps back to the lowest step (**20%** with default settings).
- Selected step is saved in EEPROM and restored after reset/power cycle.
- PWM output is generated on **D9** at about **25 kHz** (common PC fan PWM frequency).

## Wiring

### Pushbutton

- One side of button to **+5V**
- Other side to **D2**
- **10kΩ pulldown** resistor from **D2** to **GND**

### Fan PWM

- Fan PWM control line to **D9** (PWM output)
- Ensure fan and Arduino share **common GND**

> Note: Standard 4-wire PC fan PWM control typically expects an open-collector/open-drain drive. This sketch outputs direct PWM from D9; if your fan requires strict compliance, use a transistor stage.

## Configuration

Edit these constants in `Arduino_PushbuttonPWM.ino`:

- `SPEED_STEPS` (`100` must be divisible by this value)
- `BUTTON_PIN`
- `FAN_PWM_PIN`
- `EEPROM_ADDR`
- `DEBOUNCE_MS`

## How it works

1. On startup, the board reads the saved speed step from EEPROM.
2. It applies the corresponding PWM duty cycle.
3. Each debounced button press increments the step and wraps at max.
4. The new step is written to EEPROM using `EEPROM.update()`.
