#pragma once
#include "ws2812b/Led.h"
#include "ws2812b/RGB.h"
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

class Drink_control{
    void setPixelColor(int pixelNumber, int r, int g, int b);
public:
    Drink_control();
    void open();
    void close();
    bool drink_ready();
    void start_light();
    
};