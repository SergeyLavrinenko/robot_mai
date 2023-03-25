#include <chrono>
#include <iostream>
#include <assert.h>
#include "pwm_controller.h"


pwm_controller::pwm_controller(GpioHandle *handle_) {
    this->pwmActive = true;
    this->isRunning = false;
    this->handle = handle_;

    this->handler = std::thread(&pwm_controller::handle_pwm, this);
}

pwm_controller::~pwm_controller() {
    this->pwmActive = false;
    this->handler.join();
}

void pwm_controller::stop() {
    this->mut.lock();
    this->freq = 0;
    this->isRunning = false;
    this->mut.unlock();
}

static void setPins(GpioHandle* handle, int l, int lrev, int rrev, int r, int a, int b) {
    GpioOut(*handle, GPIO_PIN_NUM_IN1, l);
    GpioOut(*handle, GPIO_PIN_NUM_IN2, lrev);
    GpioOut(*handle, GPIO_PIN_NUM_IN3, rrev);
    GpioOut(*handle, GPIO_PIN_NUM_IN4, r);
    GpioOut(*handle, GPIO_PIN_NUM_ENA, a);
    GpioOut(*handle, GPIO_PIN_NUM_ENB, b);
}

void pwm_controller::set_pins(int perc, int l, int lrev, int rrev, int r) {
    this->mut.lock();
    this->pin_L = l;
    this->pin_LRev = lrev;
    this->pin_RRev = rrev;
    this->pin_R = r;
    this->freq = perc;
    this->isRunning = true;
    this->mut.unlock();
}

void pwm_controller::goLeft(int perc) {
    set_pins(perc, 1, 0, 1, 0);
}

void pwm_controller::goRight(int perc) {
    set_pins(perc, 0, 1, 0, 1);
}

void pwm_controller::goForward(int perc) {
    set_pins(perc, 1, 0, 0, 1);
}

void pwm_controller::goBack(int perc) {
    set_pins(perc, 0, 1, 1, 0);
}

#define TICK_PERIOD_US 50
#define TICK_PER_CIRCLE 100

void pwm_controller::handle_pwm() {
    int l, lrev, rrev, r, freq_;
    bool run;
    while (this->pwmActive) {
        this->mut.lock();
        l = this->pin_L;
        lrev = this->pin_LRev;
        rrev = this->pin_RRev;
        r = this->pin_R;
        freq_ = this->freq;
        run = this->isRunning;
        this->mut.unlock();
         if (run) {
            auto sleep_a = freq_ * TICK_PERIOD_US;
            auto sleep_b = (TICK_PER_CIRCLE - freq_) * TICK_PERIOD_US;
            if (sleep_a > 0) {
                setPins(handle, l, lrev, rrev, r, 1, 1);
                usleep(sleep_a);
            } 
            if (sleep_b > 0) {
                 setPins(handle, 0, 0, 0, 0, 1, 1);
                 usleep(sleep_b);
            }

         } else {
            setPins(handle, 0, 0, 0, 0, 0, 0);
            usleep(TICK_PER_CIRCLE * TICK_PERIOD_US);
         }
        
    }
}
