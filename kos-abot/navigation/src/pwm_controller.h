#ifndef _NAVIGATION_PVM_CONTROLLER_H
#define _NAVIGATION_PVM_CONTROLLER_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <gpio/gpio.h>
#include <stdbool.h>


#include <bsp/bsp.h>

#include <thread>
#include <unistd.h>
#include <mutex>

#define GPIO_PIN_NUM_IN2 12U
#define GPIO_PIN_NUM_IN1 13U
#define GPIO_PIN_NUM_ENA 6U
#define GPIO_PIN_NUM_IN4 20U
#define GPIO_PIN_NUM_IN3 21U
#define GPIO_PIN_NUM_ENB 26U

#define HIGH 1
#define LOW 0


class pwm_controller {
    int freq; // 0 - 99
    
    bool isRunning;
    bool pwmActive;

    int pin_L, pin_LRev;
    int pin_R, pin_RRev;
    GpioHandle *handle;

    std::thread handler;
    std::mutex mut;

    void handle_pwm();
    void set_pins(int perc, int l, int lrev, int rrev, int r);

public:
    pwm_controller(GpioHandle *handle);
    ~pwm_controller();
    
    void goLeft(int perc);
    void goRight(int perc);
    void goForward(int perc);
    void goBack(int perc);


    void stop();
};

#endif