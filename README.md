# ESP8266 door lock sensor

## Overview

This small piece of code is PoC of detecting door lock state with GY-271 (QMC5883L) magnetometer

## How to install this code on ESP8266

### MOS CLI tool
- Install [mos tool](https://mongoose-os.com/software.html)
- Clone this project with `$ git clone https://github.com/infrat/esp8266-door-lock.git`
- Inside cloned directory execute:
  - `$ mos build --arch=esp8266`
  - `$ mod flash --port=<port_of_your_esp8266>` 

## Hardware
- This firmware is configured to use following peripherals
  - ESP12F (other ESP8266 based boards should also work after picking available GPIOs in source code)
  - GY-271 magnetometer module based on QMC5883L (please note that HMC5883L != QMC5883L; to use HMC5883L some changes must be performed in source code)
  - SSD1306 OLED display (I2C) to show magnetic field measurments
  - two switches to calibrate current door lock state: 
    - LOCK1 switch connecting GND with GPIO5, 
    - LOCK2 switch connecting GND with GPIO4

All I2C peripherals should be connected to GPIO12 (SDA) and GPIO14 (SCL)

## Usage
1. Mount GY-271 sensor on door frame possibly close to lock bolt.
2. To improve magnetic field around your door lock, glue small neodymium magnet to door lock bolt.

After booting the firmware, you should see raw measurments of 3 axis magnetic fields around the sensor. 
Place the door lock in first position, and push LOCK1 switch to save the measurments, and identify them as LOCK1 state. Do the same with the second door lock position, and push LOCK2 switch. The text in the last line on OLED display should determine current lock state (OPEN, LOCK1, LOCK2)

If door lock state is not recognized properly, try with rotating magnet mounted on bolt around one of 3 axis, or try to rotate sensor (also in one of 3 axis).

