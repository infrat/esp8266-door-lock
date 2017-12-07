#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include <iostream>
#include <sstream>
#include <string>

#include "common/cs_dbg.h"
#include "mgos_app.h"
#include "mgos_timers.h"
#include "Wire.h"
#include "mgos_gpio.h"

#define QMC5883L_ADDR 0x0D
#define DIV 10
#define PRECISION 30

Adafruit_SSD1306 *d1 = nullptr;

static void timer_cb(void *arg);

struct Lock {
    int x;
    int y;
    int z;
};

struct Lock Lock1;
struct Lock Lock2;

void setupOLED(void) {
    d1 = new Adafruit_SSD1306(4, Adafruit_SSD1306::RES_128_64);

    d1->begin(SSD1306_SWITCHCAPVCC, 0x3C, true);
    d1->display();

    Wire.begin();
    Wire.beginTransmission(QMC5883L_ADDR); //start talking
    Wire.write(0x0B); // Tell the HMC5883 to Continuously Measure
    Wire.write(0x01); // Set the Register
    Wire.endTransmission();

    Wire.beginTransmission(QMC5883L_ADDR); //start talking
    Wire.write(0x09); // Tell the HMC5883 to Continuously Measure
    Wire.write(0x1D); // Set the Register
    Wire.endTransmission();
}

void setupGPIO() {
    mgos_gpio_set_mode(4, MGOS_GPIO_MODE_INPUT);
    mgos_gpio_set_mode(5, MGOS_GPIO_MODE_INPUT);
    mgos_gpio_set_pull(4, MGOS_GPIO_PULL_UP);
    mgos_gpio_set_pull(5, MGOS_GPIO_PULL_UP);
}

enum mgos_app_init_result mgos_app_init(void) {
    setupOLED();
    setupGPIO();
    mgos_set_timer(100 /* ms */, true /* repeat */, timer_cb, NULL);
    return MGOS_APP_INIT_SUCCESS;
}

static void show_num(Adafruit_SSD1306 *d, int x, int y, std::string name) {
    d->clearDisplay();
    d->setTextSize(1);
    d->setTextColor(WHITE);
    d->setCursor(x, y);
    d->printf(name.c_str());
    d->display();
}

bool isCloseTo(int a, int b) {
    return ((a < b + PRECISION) && (a > b - PRECISION));
}

static void readQMC(int *x, int *y, int *z) {
    Wire.beginTransmission(QMC5883L_ADDR);
    Wire.write(0x00);
    Wire.endTransmission();

    Wire.requestFrom(QMC5883L_ADDR, 6);

    if (6 <= Wire.available()) {
        *x = (Wire.read() | (Wire.read()<<8)) / DIV;
        *y = (Wire.read() | (Wire.read()<<8)) / DIV;
        *z = (Wire.read() | (Wire.read()<<8)) / DIV;
    }
}

static void timer_cb(void *arg) {
    int x = 0, y = 0, z = 0;
    bool gpio4 = !mgos_gpio_read(4);
    bool gpio5 = !mgos_gpio_read(5);
    std::ostringstream oledOut;
    readQMC(&x, &y, &z);

    if (gpio5) {
        Lock1.x = x;
        Lock1.y = y;
        Lock1.z = z;
    }

    if (gpio4) {
        Lock2.x = x;
        Lock2.y = y;
        Lock2.z = z;
    }

    oledOut << "curr: " << x << "." << y << "." << z << "\n";
    oledOut << "lck1: " << Lock1.x << "." << Lock1.y << "." << Lock1.z << "\n";
    oledOut << "lck2: " << Lock2.x << "." << Lock2.y << "." << Lock2.z << "\n";
    oledOut << "gpio4: " << gpio4 << " gpio5:" << gpio5 << "\n";
    oledOut << "\n";

    if (isCloseTo(Lock1.x, x) && isCloseTo(Lock1.y, y) && isCloseTo(Lock1.z, z)) {
        oledOut << "LOCK1\n";
    } else if (isCloseTo(Lock2.x, x) && isCloseTo(Lock2.y, y) && isCloseTo(Lock2.z, z)) {
        oledOut << "LOCK2\n";
    } else {
        oledOut << "OPEN\n";
    }

    show_num(d1, 0, 10, oledOut.str());
    LOG(LL_INFO, ("x = %d, y = %d, z = %d", x, y, z));
  (void) arg;
}

#if 0
void loop(void) {
  /* For now, do not use delay() inside loop, use timers instead. */
}
#endif
